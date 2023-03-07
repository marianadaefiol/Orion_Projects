#include "glue.hh"
#include <p24FJ64GA006.h>

namespace
{
  // Variables for the Rx interrupt handler

  // 1 if the request was received by the Rx interrupt handler, 0 otherwise.
  volatile int unvalidatedRequestReceived;
  //int uDB25;

  // Describes the current step
  enum ReceiveStep
  {
    WaitingForSync,
    WaitingForSize,
    WaitingForSeq,
    WaitingForData,
    WaitingForCrc
  } receiveStep;

  // Indicates that an escape byte was received
  int escapeReceived;
  int uDB25; 

  // WaitingForSize step
  int sizeBytesReceived;
  uint16_t receivedSize;

  // WaitingForSeq step
  int seqBytesReceived;
  uint16_t receivedSeq;

  // WaitingForData step
  uint16_t receiveBytesLeft;
  unsigned char * nextByteToReceive;

  // WaitingForCrc step
  int crcBytesReceived;
  uint16_t receivedCrc;

  // Variables for the Tx interrupt handler

  // Indicates that a new event message should be sent after the previous
  // message transmission completes.
  volatile int eventMessagePending;

  // Indicates that the event message sending is in progress, and accounts
  // for a number of bytes of an event message which is still to be sent.
  volatile int eventMessageBytesLeft;

  // Describes the current step
  enum SendStep
  {
    SendingInactive, // This one means that no sending is in progress
    SendingSync,
    SendingSize,
    SendingSeq,
    SendingRequestSeq,
    SendingData,
    SendingCrc,
    SendingFinished
  } sendStep;

  // Indicates that an escape byte was sent
  int escapeSent;
  // The byte which should be sent after the escape byte
  unsigned char escapeByte;

  int sizeBytesSent;
  uint16_t sizeToSend;

  int seqBytesSent;
  uint16_t seqToSend;

  int requestSeqBytesSent;
  uint16_t requestSeqToSend;

  uint16_t sendBytesLeft;
  unsigned char * nextByteToSend;

  int crcBytesSent;
  uint16_t crcToSend;

  // The bottom-halves' part

  // 1 if the request was successfully received and checked for validity, 0
  // otherwise.
  unsigned int requestReceived;

  // Previously received seq, 0 if none.
  uint16_t previousSeq;
  
}

namespace MODULE {

void init()
{
  unvalidatedRequestReceived = 0;
  receiveStep = WaitingForSync;

 
  previousSeq = 0;

  sendStep = SendingInactive;

  eventMessageBytesLeft = 0;

  // Init UART1 and UART2 for the main communication with the PC

  // Set the interrupts' priority to 2
  IPC2bits.U1RXIP = 5;
  IPC3bits.U1TXIP = 5;
  IPC7bits.U2RXIP = 1;
  IPC7bits.U2TXIP = 1;

  // Clear the interupt status flags
  IFS0bits.U1RXIF = 0;
  IFS0bits.U1TXIF = 0;
  IFS1bits.U2RXIF = 0;
  IFS1bits.U2TXIF = 0;

  // Enable the rx interrupt, disable the tx interrupt.
  IEC0bits.U1RXIE = 1;
  IEC0bits.U1TXIE = 0;  
  IEC1bits.U2RXIE = 1;
  IEC1bits.U2TXIE = 0;

  // Configure UART2
  TRISFbits.TRISF2 = 1; // Rx is In
  TRISFbits.TRISF3 = 0; // Tx is Out
  TRISFbits.TRISF4 = 1; // Rx is In
  TRISFbits.TRISF5 = 0; // Tx is Out

  U1BRG  = 21; // 113636 baud at 20Mhz clock. WARNING: RevA3 errata may apply!
  U1MODE = (1<<15) | (1<<3);  // UARTEN | BRGH

  U2BRG  = 21; // 259;;; 113636 baud at 20Mhz clock. WARNING: RevA3 errata may apply! //21
  U2MODE = (1<<15) | (1<<3);  // UARTEN | BRGH

  U1STA = 1<<10;   // UTXEN
  U2STA = 1<<10;   // UTXEN
  //uDB25 = false;
}

namespace {

// Starts the sending process by kickstarting interrupt handling if it was
// inactive

void startSending()
{
   if ( !IEC0bits.U1TXIE && uDB25 == 0 )
   //if (uDB25 == 0) 
  {
    // If the interrupt is inactive, it means nothing is transmitted now
    // and is not going to be transmitted later unless we enable it.

    // Wait until there's a possibility to transmit a byte. Normally here
    // the queue is always clear, but we check just to be sure.
     while( U1STAbits.UTXBF );

    // Artificially raise the interrupt flag and enable the interrupt itself
    // to start the transmission
    IFS0bits.U1TXIF = 1;
    IEC0bits.U1TXIE = 1;
  }
   else 
  {
    while( U2STAbits.UTXBF );
    IFS1bits.U2TXIF = 1;
    IEC1bits.U2TXIE = 1;
  }
}
}

uint16_t isRequestReceived()
{
  if ( requestReceived )
    return receivedSize; // Nothing to do here, we already done this on the
                         // previous run

  if ( unvalidatedRequestReceived )
  {
    // We have a pending request awaiting to be validated.

    // Decrypt it and check its crc. We do two these steps simultaneously.
    
    // If the message size was odd, we add one zero byte to its end

    if ( receivedSize & 1 )
      requestBody[ receivedSize ] = 0;

    Crc::init();

    Crc::addOne( receivedSize );
    Crc::addOne( receivedSeq );

    uint32_t seed = Protocol::initialSeedValue( receivedSeq );

    int wordsLeft = ( receivedSize + 1 )/2;

    uint16_t * nextWord = (uint16_t *) requestBody;

    for( ; wordsLeft--; ++nextWord )
    {
      Protocol::nextGamma( seed );

      *nextWord ^= ( seed & 0xFFFF );

      Crc::addOne( *nextWord );
    }

    Crc::finalize();

    // Ok, by now the message should be decrypted and the crc should be
    // counted.

    if ( Crc::get() != receivedCrc )
    {
      // Bad crc, discard the packet
      unvalidatedRequestReceived = 0;

      return 0;
    }
    else
    {
      // Crc is good. We will have to handle the request one way or another.

      // If there was any outgoing response being sent, stop it now, we're
      // having some out-of-order request.
      sendStep = SendingInactive;
      
      // Now, if the sequence matches the sequence of the previously received
      // request, we resend the response we already have.
      if ( receivedSeq == previousSeq )
      {
        // Resend the response

        unvalidatedRequestReceived = 0;

        sendStep = SendingSync;

        startSending();

        return 0;
      }
      else
      {
        // Let the outside process the request

        requestReceived = 1;

        return receivedSize;
      }
    }
  }
  else
    return 0; // We have received nothing, nothing to handle.
}

void sendReply( uint16_t size )
{
  // Which seq will we use?

  do
  {
    seqToSend = Rand::rand() & 0xFFFF;
  } while( !seqToSend );

  sizeToSend = size;

  // Mangle the seq to protect from the statistical gamma recovery attack
  requestSeqToSend = receivedSeq ^ Protocol::RequestSeqGamma;

  // Calculate crc and encrypt the message

  Crc::init();

  Crc::addOne( size );
  Crc::addOne( seqToSend );

  // If the message size is odd, pad it with a zero byte

  if ( size & 1 )
    responseBody[ size ] = 0;

  uint32_t seed = Protocol::initialSeedValue( seqToSend );

  Crc::addOne( requestSeqToSend );

  Protocol::nextGamma( seed );

  requestSeqToSend ^= ( seed & 0xFFFF );

  int wordsLeft = ( size + 1 )/2;

  uint16_t * nextWord = (uint16_t *) responseBody;

  for( ; wordsLeft--; ++nextWord )
  {
    Crc::addOne( *nextWord );

    Protocol::nextGamma( seed );

    *nextWord ^= ( seed & 0xFFFF );
  }

  Crc::finalize();

  crcToSend = Crc::get();

  // Ok, now everything is ready, finalize the state and start sending.

  previousSeq = receivedSeq;

  // We drop the request we have, allowing for a new one to be received.
  requestReceived = 0;
  receiveStep = WaitingForSync;
  unvalidatedRequestReceived = 0;

  sendStep = SendingSync;

  startSending();
}

void sendEventMessage()
{
  // First, we set it here to make any active interrupt handler pick it up.
  eventMessagePending = 1;

  // Now, check if we should kickstart the interrupt handling
  //~if ( !IEC1bits.U2TXIE )
  //if ( !IEC0bits.U1TXIE )
  if ( !IEC0bits.U1TXIE || !IEC1bits.U2TXIE )
  {
    // The interrupt is inactive

    if ( eventMessagePending )
    {
      // And the event message is still pending -- we should start sending

      // The interrupt handler can't pick up an event pending from the
      // dry start, it can only do that if it was sending other stuff before.
      // So we convert an event pending to an event being sent.
      
      eventMessagePending = 0;
      eventMessageBytesLeft = 9;

      startSending();
    }
  }
  /*
  else if (!IEC1bits.U2TXIE )
  {

    if ( eventMessagePending )
    {
      eventMessagePending = 0;
      eventMessageBytesLeft = 9;

      startSending();
    }
  } 
  */
}

unsigned char requestBody[ 512 ];
unsigned char responseBody[ 512 ];

}


extern "C" {

// Rx handler
//~void __attribute__((__interrupt__,no_auto_psv)) _U2RXInterrupt( void )
void __attribute__((__interrupt__,no_auto_psv)) _U1RXInterrupt( void )
{
  // Clear the interrupt, we're handling it.
  //~IFS1bits.U2RXIF = 0;
   IFS0bits.U1RXIF = 0;

  //LATFbits.LATF0 = 1;
  //LATFbits.LATF1 = 0;
  
  //LATE = receiveStep + 1;

  // What should we do with the bytes?

  if ( unvalidatedRequestReceived )
  {
    // We're handling one request already, we have to waste all the bytes,
    // sorry.
    //~while( U2STA & 1 )
     while( U1STA & 1 )
    {
      //~U2RXREG;
      U1RXREG;
    }
  }
  else
  {
    // Handle the incoming bytes.
    //~while( U2STAbits.URXDA )
    while( U1STAbits.URXDA )
    {
      //~unsigned char sym = U2RXREG;
      unsigned char sym = U1RXREG;

      if ( sym == MODULE::Protocol::Sync )
      {
        // A sync byte, we should (re)start receiving the message.
        receiveStep = WaitingForSize;
        sizeBytesReceived = 0;
        escapeReceived = 0;
        continue;
      }

      // Handle the escaping now

      if ( escapeReceived )
      {
        escapeReceived = 0;

        switch( sym )
        {
          case MODULE::Protocol::Escaped_AA:
            sym = 0xAA;
          break;

          case MODULE::Protocol::Escaped_BB:
            sym = 0xBB;
          break;

          default:
            // Invalid escape sequence, ignore the whole thing and the
            // whole message we received so far as well.
            receiveStep = WaitingForSync;
            continue;
        }
      }
      else
      if ( sym == MODULE::Protocol::Escape )
      {
        escapeReceived = 1;
        continue;
      }

      switch( receiveStep )
      {
        case WaitingForSync:
          // Do nothing, no sync byte was received.
        break;

        case WaitingForSize:
          if ( !sizeBytesReceived )
          {
            // Receive the first byte of the size
            receivedSize = sym;
            sizeBytesReceived = 1;
          }
          else
          {
            // Receive the second byte of the size
            receivedSize |= ( (uint16_t) sym ) << 8;

            if ( receivedSize > sizeof( MODULE::requestBody ) || !receivedSize )
            {
              // The message is invalid, the size can't be that large, or 0.
              receiveStep = WaitingForSync;
            }
            else
            {
              receiveStep = WaitingForSeq;
              seqBytesReceived = 0;
            }
          }
        break;

        case WaitingForSeq:
          if ( !seqBytesReceived )
          {
            // Receive the first byte of the seq
            receivedSeq = sym;
            seqBytesReceived = 1;
          }
          else
          {
            // Receive the second byte of the seq
            receivedSeq |= ( (uint16_t) sym ) << 8;

            if ( !receivedSeq )
            {
              // The message is invalid, the seq can't be zero.
              receiveStep = WaitingForSync;
            }
            else
            {
              receiveStep = WaitingForData;
              receiveBytesLeft = receivedSize;
              nextByteToReceive = MODULE::requestBody;
            }
          }
        break;

        case WaitingForData:
          *nextByteToReceive++ = sym;
          --receiveBytesLeft;

          if ( !receiveBytesLeft )
          {
            // Received all the bytes, now the CRC should follow.
            receiveStep = WaitingForCrc;
            crcBytesReceived = 0;
          }
        break;

        case WaitingForCrc:
          if ( !crcBytesReceived )
          {
            // Receive the first byte of the CRC
            receivedCrc = sym;
            crcBytesReceived = 1;
          }
          else
          {
            // Receive the second byte of the CRC
            receivedCrc |= ( (uint16_t) sym ) << 8;

            // Ok, we get everything, now let the bottom-half handle the
            // rest.

            unvalidatedRequestReceived = 1;

            return;
          }
        break;
      } // switch( receiveStep )
    } // while( where is a next byte )
  }
}


 void __attribute__((__interrupt__,no_auto_psv)) _U1TXInterrupt( void )
{
  // This interrupt handler can only be called when we're in the process
  // of sending, otherwise it is turned off.

  // Clear the interrupt, we're handling it.
  //~IFS1bits.U2TXIF = 0;
   IFS0bits.U1TXIF = 0;

  //LATFbits.LATF0 = 0;
  //LATFbits.LATF1 = 1;

  if ( !eventMessageBytesLeft && sendStep == SendingInactive )
  {
    // The sending was apparently deactivated in the middle, we should either
    // proceed with the pending event message, or stop everyting.

    if ( eventMessagePending )
    {
      eventMessageBytesLeft = 9;
      eventMessagePending = 0;
    }
    else
    {
      // No event message, disable the interrupt
      //~IEC1bits.U2TXIE = 0;
      IEC0bits.U1TXIE = 0;

      return;
    }
  }

  //~while( !U2STAbits.UTXBF )
   while( !U1STAbits.UTXBF )
  {
    // We can send at least one more character

    if ( eventMessageBytesLeft )
    {
      // We're sending the event message

      if ( eventMessageBytesLeft == 9 )
        //~U2TXREG = MODULE::Protocol::Sync; // First byte is sync
         U1TXREG = MODULE::Protocol::Sync; // First byte is sync
      else
        //~U2TXREG = 0; // The rest of the body
         U1TXREG = 0; // The rest of the body

      if ( !--eventMessageBytesLeft )
      {
        // Since this was the last byte of an event message, any pending
        // events raised before it was sent should now be cancelled.
        eventMessagePending = 0;

        // We've finished sending the event message -- if there is a normal
        // message pending, start sending it now.
        if ( sendStep != SendingSync )
        {
          // No message, disable the interrupt
          //~IEC1bits.U2TXIE = 0;
           IEC0bits.U1TXIE = 0;


          return;
        }
      }

      continue;
    }

    unsigned char sym = 0; // No need to init but to please the compiler

    if ( sendStep == SendingSync )
    {
      // We're sending the sync byte and setting for the future. The
      // sync byte should not be escaped.
      
      //~U2TXREG = MODULE::Protocol::Sync;
      U1TXREG = MODULE::Protocol::Sync;
      escapeSent = 0;
      sendStep = SendingSize;
      sizeBytesSent = 0;

      continue;
    }

    if ( escapeSent )
    {
      // Send the byte which should go after the escape
      //~U2TXREG = escapeByte;
       U1TXREG = escapeByte;

      escapeSent = 0;

      if ( sendStep == SendingFinished )
      { 
        // We've sent the escaped byte, no further bytes to send unless
        // there's an event message pending.

        if ( eventMessagePending )
        {
          eventMessageBytesLeft = 9;
          eventMessagePending = 0;

          continue;
        }

        //~IEC1bits.U2TXIE = 0;
        IEC0bits.U1TXIE = 0;

        return;
      }
      continue;
    }

    switch( sendStep )
    {
      case SendingSize:
        if ( !sizeBytesSent )
        {
          sym = sizeToSend & 0xFF;
          sizeBytesSent = 1;
        }
        else
        {
          sym = sizeToSend >> 8;
          sendStep = SendingSeq;
          seqBytesSent = 0;
        }
      break;

      case SendingSeq:
        if ( !seqBytesSent )
        {
          sym = seqToSend & 0xFF;
          seqBytesSent = 1;
        }
        else
        {
          sym = seqToSend >> 8;
          sendStep = SendingRequestSeq;
          requestSeqBytesSent = 0;
        }
      break;

      case SendingRequestSeq:
        if ( !requestSeqBytesSent )
        {
          sym = requestSeqToSend & 0xFF;
          requestSeqBytesSent = 1;
        }
        else
        {
          sym = requestSeqToSend >> 8;

          if ( sizeToSend )
          {
            sendStep = SendingData;
            sendBytesLeft = sizeToSend;
            nextByteToSend = MODULE::responseBody;
          }
          else
          {
            sendStep = SendingCrc;
            crcBytesSent = 0;
          }
        }
      break;

      case SendingData:

        sym = *nextByteToSend++;

        if ( !--sendBytesLeft )
        {
          sendStep = SendingCrc;
          crcBytesSent = 0;
        }

      break;

      case SendingCrc:

        if ( !crcBytesSent )
        {
          sym = crcToSend & 0xFF;
          crcBytesSent = 1;
        }
        else
        {
          sym = crcToSend >> 8;

          // That's the last byte, we're finished.

          sendStep = SendingFinished;
        }
      break;

      default:
        break;
    } // switch( sendStep )

    // Do we need to escape the byte we're sending?

    if ( sym == MODULE::Protocol::Sync || sym == MODULE::Protocol::Escape )
    {
      // Yes, we do.
      if ( sym == MODULE::Protocol::Sync )
        escapeByte = MODULE::Protocol::Escaped_AA;
      else
        escapeByte = MODULE::Protocol::Escaped_BB;

      sym = MODULE::Protocol::Escape;

      escapeSent = 1;
    }

    // Send the byte

    //~U2TXREG = sym;
     U1TXREG = sym;

    if ( !escapeSent && sendStep == SendingFinished )
    {
      // We didn't escape the last byte, no further bytes to send unless
      // there's an event message pending.
      
      if ( eventMessagePending )
      {
        eventMessageBytesLeft = 9;
        eventMessagePending = 0;

        continue;
      }

      //~IEC1bits.U2TXIE = 0;
       IEC0bits.U1TXIE = 0;
      return;
    }
  }
}


void __attribute__((__interrupt__,no_auto_psv)) _U2RXInterrupt( void )
{
  uDB25 = 1;

  // Clear the interrupt, we're handling it.
  IFS1bits.U2RXIF = 0;
  //~IFS0bits.U1RXIF = 0;

  //LATFbits.LATF0 = 1;
  //LATFbits.LATF1 = 0;
  
  //LATE = receiveStep + 1;

  // What should we do with the bytes?

  if ( unvalidatedRequestReceived )
  {
    // We're handling one request already, we have to waste all the bytes,
    // sorry.
    
    while( U2STA & 1 )
    {
      U2RXREG;
    }
  }
  else
  {
    // Handle the incoming bytes.
    while( U2STAbits.URXDA )
    {
      unsigned char sym = U2RXREG;

      if ( sym == MODULE::Protocol::Sync )
      {
        // A sync byte, we should (re)start receiving the message.
        receiveStep = WaitingForSize;
        sizeBytesReceived = 0;
        escapeReceived = 0;
        continue;
      }

      // Handle the escaping now

      if ( escapeReceived )
      {
        escapeReceived = 0;

        switch( sym )
        {
          case MODULE::Protocol::Escaped_AA:
            sym = 0xAA;
          break;

          case MODULE::Protocol::Escaped_BB:
            sym = 0xBB;
          break;

          default:
            // Invalid escape sequence, ignore the whole thing and the
            // whole message we received so far as well.
            receiveStep = WaitingForSync;
            continue;
        }
      }
      else
      if ( sym == MODULE::Protocol::Escape )
      {
        escapeReceived = 1;
        continue;
      }

      switch( receiveStep )
      {
        case WaitingForSync:
          // Do nothing, no sync byte was received.
        break;

        case WaitingForSize:
          if ( !sizeBytesReceived )
          {
            // Receive the first byte of the size
            receivedSize = sym;
            sizeBytesReceived = 1;
          }
          else
          {
            // Receive the second byte of the size
            receivedSize |= ( (uint16_t) sym ) << 8;

            if ( receivedSize > sizeof( MODULE::requestBody ) || !receivedSize )
            {
              // The message is invalid, the size can't be that large, or 0.
              receiveStep = WaitingForSync;
            }
            else
            {
              receiveStep = WaitingForSeq;
              seqBytesReceived = 0;
            }
          }
        break;

        case WaitingForSeq:
          if ( !seqBytesReceived )
          {
            // Receive the first byte of the seq
            receivedSeq = sym;
            seqBytesReceived = 1;
          }
          else
          {
            // Receive the second byte of the seq
            receivedSeq |= ( (uint16_t) sym ) << 8;

            if ( !receivedSeq )
            {
              // The message is invalid, the seq can't be zero.
              receiveStep = WaitingForSync;
            }
            else
            {
              receiveStep = WaitingForData;
              receiveBytesLeft = receivedSize;
              nextByteToReceive = MODULE::requestBody;
            }
          }
        break;

        case WaitingForData:
          *nextByteToReceive++ = sym;
          --receiveBytesLeft;

          if ( !receiveBytesLeft )
          {
            // Received all the bytes, now the CRC should follow.
            receiveStep = WaitingForCrc;
            crcBytesReceived = 0;
          }
        break;

        case WaitingForCrc:
          if ( !crcBytesReceived )
          {
            // Receive the first byte of the CRC
            receivedCrc = sym;
            crcBytesReceived = 1;
          }
          else
          {
            // Receive the second byte of the CRC
            receivedCrc |= ( (uint16_t) sym ) << 8;

            // Ok, we get everything, now let the bottom-half handle the
            // rest.

            unvalidatedRequestReceived = 1;

            return;
          }
        break;
      } // switch( receiveStep )
    } // while( where is a next byte )
  }
}


void __attribute__((__interrupt__,no_auto_psv)) _U2TXInterrupt( void )
{
  // This interrupt handler can only be called when we're in the process
  // of sending, otherwise it is turned off.

  // Clear the interrupt, we're handling it.
  IFS1bits.U2TXIF = 0;

  //LATFbits.LATF0 = 0;
  //LATFbits.LATF1 = 1;

  if ( !eventMessageBytesLeft && sendStep == SendingInactive )
  {
    // The sending was apparently deactivated in the middle, we should either
    // proceed with the pending event message, or stop everyting.

    if ( eventMessagePending )
    {
      eventMessageBytesLeft = 9;
      eventMessagePending = 0;
    }
    else
    {
      // No event message, disable the interrupt
      IEC1bits.U2TXIE = 0;

      return;
    }
  }

  while( !U2STAbits.UTXBF )
  {
    // We can send at least one more character

    if ( eventMessageBytesLeft )
    {
      // We're sending the event message

      if ( eventMessageBytesLeft == 9 )
         U2TXREG = MODULE::Protocol::Sync; // First byte is sync
      else
         U2TXREG = 0; // The rest of the body

      if ( !--eventMessageBytesLeft )
      {
        // Since this was the last byte of an event message, any pending
        // events raised before it was sent should now be cancelled.
        eventMessagePending = 0;

        // We've finished sending the event message -- if there is a normal
        // message pending, start sending it now.
        if ( sendStep != SendingSync )
        {
          // No message, disable the interrupt
           IEC1bits.U2TXIE = 0;


          return;
        }
      }

      continue;
    }

    unsigned char sym = 0; // No need to init but to please the compiler

    if ( sendStep == SendingSync )
    {
      // We're sending the sync byte and setting for the future. The
      // sync byte should not be escaped.
      
      U2TXREG = MODULE::Protocol::Sync;
      escapeSent = 0;
      sendStep = SendingSize;
      sizeBytesSent = 0;

      continue;
    }

    if ( escapeSent )
    {
      // Send the byte which should go after the escape
       U2TXREG = escapeByte;

      escapeSent = 0;

      if ( sendStep == SendingFinished )
      { 
        // We've sent the escaped byte, no further bytes to send unless
        // there's an event message pending.

        if ( eventMessagePending )
        {
          eventMessageBytesLeft = 9;
          eventMessagePending = 0;

          continue;
        }

        IEC1bits.U2TXIE = 0;

        return;
      }
      continue;
    }

    switch( sendStep )
    {
      case SendingSize:
        if ( !sizeBytesSent )
        {
          sym = sizeToSend & 0xFF;
          sizeBytesSent = 1;
        }
        else
        {
          sym = sizeToSend >> 8;
          sendStep = SendingSeq;
          seqBytesSent = 0;
        }
      break;

      case SendingSeq:
        if ( !seqBytesSent )
        {
          sym = seqToSend & 0xFF;
          seqBytesSent = 1;
        }
        else
        {
          sym = seqToSend >> 8;
          sendStep = SendingRequestSeq;
          requestSeqBytesSent = 0;
        }
      break;

      case SendingRequestSeq:
        if ( !requestSeqBytesSent )
        {
          sym = requestSeqToSend & 0xFF;
          requestSeqBytesSent = 1;
        }
        else
        {
          sym = requestSeqToSend >> 8;

          if ( sizeToSend )
          {
            sendStep = SendingData;
            sendBytesLeft = sizeToSend;
            nextByteToSend = MODULE::responseBody;
          }
          else
          {
            sendStep = SendingCrc;
            crcBytesSent = 0;
          }
        }
      break;

      case SendingData:

        sym = *nextByteToSend++;

        if ( !--sendBytesLeft )
        {
          sendStep = SendingCrc;
          crcBytesSent = 0;
        }

      break;

      case SendingCrc:

        if ( !crcBytesSent )
        {
          sym = crcToSend & 0xFF;
          crcBytesSent = 1;
        }
        else
        {
          sym = crcToSend >> 8;

          // That's the last byte, we're finished.

          sendStep = SendingFinished;
        }
      break;

      default:
        break;
    } // switch( sendStep )

    // Do we need to escape the byte we're sending?

    if ( sym == MODULE::Protocol::Sync || sym == MODULE::Protocol::Escape )
    {
      // Yes, we do.
      if ( sym == MODULE::Protocol::Sync )
        escapeByte = MODULE::Protocol::Escaped_AA;
      else
        escapeByte = MODULE::Protocol::Escaped_BB;

      sym = MODULE::Protocol::Escape;

      escapeSent = 1;
    }

    // Send the byte

     U2TXREG = sym;

    if ( !escapeSent && sendStep == SendingFinished )
    {
      // We didn't escape the last byte, no further bytes to send unless
      // there's an event message pending.
      
      if ( eventMessagePending )
      {
        eventMessageBytesLeft = 9;
        eventMessagePending = 0;

        continue;
      }

       IEC1bits.U2TXIE = 0;
       
      return;
    }
  }
  uDB25 = 0;
}

}