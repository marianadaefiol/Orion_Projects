
#ifndef SHA2_ROUTINES_H
#define SHA2_ROUTINES_H

#include <stdint.h>
#include <stddef.h>

#define SHA256_DIGEST_SIZE (32)
#define SHA256_BLOCK_SIZE  (64)

#define ATCA_KEY_SIZE               (32)                                //!< size of a symmetric SHA key

typedef struct
{
    uint32_t total_msg_size;                //!< Total number of message bytes processed
    uint32_t block_size;                    //!< Number of bytes in current block
    uint8_t  block[SHA256_BLOCK_SIZE * 2];  //!< Unprocessed message storage
    uint32_t hash[8];                       //!< Hash state
} sw_sha256_ctx;

void sw_sha256_init(sw_sha256_ctx* ctx);

void sw_sha256_update(sw_sha256_ctx* ctx, const uint8_t* message, uint32_t len);

void sw_sha256_final(sw_sha256_ctx * ctx, uint8_t digest[SHA256_DIGEST_SIZE]);

void sw_sha256(const uint8_t * message, unsigned int len, uint8_t digest[SHA256_DIGEST_SIZE]);


#define ATCA_SHA2_256_DIGEST_SIZE (32)

typedef struct
{
    uint32_t pad[48]; //!< Filler value to make sure the actual implementation has enough room to store its context. uint32_t is used to remove some alignment warnings.
} atcac_sha2_256_ctx;

int atcac_sw_sha2_256_init(atcac_sha2_256_ctx* ctx);
int atcac_sw_sha2_256_update(atcac_sha2_256_ctx* ctx, const uint8_t* data, size_t data_size);
int atcac_sw_sha2_256_finish(atcac_sha2_256_ctx * ctx, uint8_t digest[ATCA_SHA2_256_DIGEST_SIZE]);
int atcac_sw_sha2_256(const uint8_t * data, size_t data_size, uint8_t digest[ATCA_SHA2_256_DIGEST_SIZE]);

#define ATCA_MSG_SIZE_NONCE            (55)


/** \brief (Key or TempKey){32} || (Challenge or TempKey){32} || OpCode{1} || Mode{1} || Param2{2}
|| (OTP0_7 or 0){8} || (OTP8_10 or 0){3} || SN8{1} || (SN4_7 or 0){4} || SN0_1{2} || (SN2_3 or 0){2}
*/
#define ATCA_MSG_SIZE_MAC              (88)
#define ATCA_MSG_SIZE_HMAC             (88)

//! KeyId{32} || OpCode{1} || Param1{1} || Param2{2} || SN8{1} || SN0_1{2} || 0{25} || TempKey{32}
#define ATCA_MSG_SIZE_GEN_DIG          (96)


//! KeyId{32} || OpCode{1} || Param1{1} || Param2{2} || SN8{1} || SN0_1{2} || 0{25} || TempKey{32}
#define ATCA_MSG_SIZE_DERIVE_KEY       (96)


//! KeyId{32} || OpCode{1} || Param1{1} || Param2{2} || SN8{1} || SN0_1{2}
#define ATCA_MSG_SIZE_DERIVE_KEY_MAC   (39)

//! KeyId{32} || OpCode{1} || Param1{1} || Param2{2}|| SN8{1} || SN0_1{2} || 0{25} || TempKey{32}
#define ATCA_MSG_SIZE_ENCRYPT_MAC      (96)

//! KeyId{32} || OpCode{1} || Param1{1} || Param2{2}|| SN8{1} || SN0_1{2} || 0{21} || PlainText{36}
#define ATCA_MSG_SIZE_PRIVWRITE_MAC    (96)

#define ATCA_COMMAND_HEADER_SIZE       ( 4)
#define ATCA_GENDIG_ZEROS_SIZE         (25)
#define ATCA_WRITE_MAC_ZEROS_SIZE      (25)
#define ATCA_PRIVWRITE_MAC_ZEROS_SIZE  (21)
#define ATCA_PRIVWRITE_PLAIN_TEXT_SIZE (36)
#define ATCA_DERIVE_KEY_ZEROS_SIZE     (25)
#define HMAC_BLOCK_SIZE                  (64)
/** @} */

/** \name Default Fixed Byte Values of Serial Number (SN[0:1] and SN[8])
   @{ */
#define ATCA_SN_0_DEF                (0x01)
#define ATCA_SN_1_DEF                (0x23)
#define ATCA_SN_8_DEF                (0xEE)
/** @} */


/** \name Definition for TempKey Mode
   @{ */
//! mode mask for MAC command when using TempKey
#define MAC_MODE_USE_TEMPKEY_MASK      ((uint8_t)0x03)
/** @} */

/** \brief Structure to hold TempKey fields
 */
typedef struct atca_temp_key
{
    uint8_t  value[ATCA_KEY_SIZE]; //!< Value of TempKey
    uint32_t key_id       : 4;     //!< If TempKey was derived from a slot or transport key (GenDig or GenKey), that key ID is saved here.
    uint32_t source_flag  : 1;     //!< Indicates id TempKey started from a random nonce (0) or not (1).
    uint32_t gen_dig_data : 1;     //!< TempKey was derived from the GenDig command.
    uint32_t gen_key_data : 1;     //!< TempKey was derived from the GenKey command (ATECC devices only).
    uint32_t no_mac_flag  : 1;     //!< TempKey was derived from a key that has the NoMac bit set preventing the use of the MAC command. Known as CheckFlag in ATSHA devices).
    uint32_t valid        : 1;     //!< TempKey is valid.
} atca_temp_key_t;


/** \struct atca_include_data_in_out
 *  \brief Input / output parameters for function atca_include_data().
 *  \var atca_include_data_in_out::p_temp
 *       \brief [out] pointer to output buffer
 *  \var atca_include_data_in_out::otp
 *       \brief [in] pointer to one-time-programming data
 *  \var atca_include_data_in_out::sn
 *       \brief [in] pointer to serial number data
 */
struct atca_include_data_in_out
{
    uint8_t *      p_temp;
    const uint8_t *otp;
    const uint8_t *sn;
    uint8_t        mode;
};


/** \struct atca_nonce_in_out
 *  \brief Input/output parameters for function atca_nonce().
 *  \var atca_nonce_in_out::mode
 *       \brief [in] Mode parameter used in Nonce command (Param1).
 *  \var atca_nonce_in_out::num_in
 *       \brief [in] Pointer to 20-byte NumIn data used in Nonce command.
 *  \var atca_nonce_in_out::rand_out
 *       \brief [in] Pointer to 32-byte RandOut data from Nonce command.
 *  \var atca_nonce_in_out::temp_key
 *       \brief [in,out] Pointer to TempKey structure.
 */
typedef struct atca_nonce_in_out
{
    uint8_t               mode;
    const uint8_t *       num_in;
    const uint8_t *       rand_out;
    struct atca_temp_key *temp_key;
} atca_nonce_in_out_t;


/** \struct atca_mac_in_out
 *  \brief Input/output parameters for function atca_mac().
 *  \var atca_mac_in_out::mode
 *       \brief [in] Mode parameter used in MAC command (Param1).
 *  \var atca_mac_in_out::key_id
 *       \brief [in] KeyID parameter used in MAC command (Param2).
 *  \var atca_mac_in_out::challenge
 *       \brief [in] Pointer to 32-byte Challenge data used in MAC command, depending on mode.
 *  \var atca_mac_in_out::key
 *       \brief [in] Pointer to 32-byte key used to generate MAC digest.
 *  \var atca_mac_in_out::otp
 *       \brief [in] Pointer to 11-byte OTP, optionally included in MAC digest, depending on mode.
 *  \var atca_mac_in_out::sn
 *       \brief [in] Pointer to 9-byte SN, optionally included in MAC digest, depending on mode.
 *  \var atca_mac_in_out::response
 *       \brief [out] Pointer to 32-byte SHA-256 digest (MAC).
 *  \var atca_mac_in_out::temp_key
 *       \brief [in,out] Pointer to TempKey structure.
 */
typedef struct atca_mac_in_out
{
    uint8_t               mode;
    uint16_t              key_id;
    const uint8_t *       challenge;
    const uint8_t *       key;
    const uint8_t *       otp;
    const uint8_t *       sn;
    uint8_t *             response;
    struct atca_temp_key *temp_key;
} atca_mac_in_out_t;


/** \struct atca_hmac_in_out
 *  \brief Input/output parameters for function atca_hmac().
 *  \var atca_hmac_in_out::mode
 *       \brief [in] Mode parameter used in HMAC command (Param1).
 *  \var atca_hmac_in_out::key_id
 *       \brief [in] KeyID parameter used in HMAC command (Param2).
 *  \var atca_hmac_in_out::key
 *       \brief [in] Pointer to 32-byte key used to generate HMAC digest.
 *  \var atca_hmac_in_out::otp
 *       \brief [in] Pointer to 11-byte OTP, optionally included in HMAC digest, depending on mode.
 *  \var atca_hmac_in_out::sn
 *       \brief [in] Pointer to 9-byte SN, optionally included in HMAC digest, depending on mode.
 *  \var atca_hmac_in_out::response
 *       \brief [out] Pointer to 32-byte SHA-256 HMAC digest.
 *  \var atca_hmac_in_out::temp_key
 *       \brief [in,out] Pointer to TempKey structure.
 */
struct atca_hmac_in_out
{
    uint8_t               mode;
    uint16_t              key_id;
    const uint8_t *       key;
    const uint8_t *       otp;
    const uint8_t *       sn;
    uint8_t *             response;
    struct atca_temp_key *temp_key;
};


/**
 *  \brief Input/output parameters for function atcah_gen_dig().
 */
typedef struct atca_gen_dig_in_out
{
    uint8_t               zone;         //!< [in] Zone/Param1 for the GenDig command
    uint16_t              key_id;       //!< [in] KeyId/Param2 for the GenDig command
    bool                  is_key_nomac; //!< [in] Set to true if the slot pointed to be key_id has the SotConfig.NoMac bit set
    const uint8_t *       sn;           //!< [in] Device serial number SN[0:8]. Only SN[0:1] and SN[8] are required though.
    const uint8_t *       stored_value; //!< [in] 32-byte slot value, config block, OTP block as specified by the Zone/KeyId parameters
    const uint8_t *       other_data;   //!< [in] 32-byte value for shared nonce zone, 4-byte value if is_key_nomac is true, ignored and/or NULL otherwise
    struct atca_temp_key *temp_key;     //!< [inout] Current state of TempKey
} atca_gen_dig_in_out_t;

/**
 *  \brief Input/output parameters for function atcah_write_auth_mac() and atcah_privwrite_auth_mac().
 */
typedef struct atca_write_mac_in_out
{
    uint8_t               zone;           //!< Zone/Param1 for the Write or PrivWrite command
    uint16_t              key_id;         //!< KeyID/Param2 for the Write or PrivWrite command
    const uint8_t *       sn;             //!< Device serial number SN[0:8]. Only SN[0:1] and SN[8] are required though.
    const uint8_t *       input_data;     //!< Data to be encrypted. 32 bytes for Write command, 36 bytes for PrivWrite command.
    uint8_t *             encrypted_data; //!< Encrypted version of input_data will be returned here. 32 bytes for Write command, 36 bytes for PrivWrite command.
    uint8_t *             auth_mac;       //!< Write MAC will be returned here. 32 bytes.
    struct atca_temp_key *temp_key;       //!< Current state of TempKey.
} atca_write_mac_in_out_t;

/**
 *  \brief Input/output parameters for function atcah_derive_key().
 */
struct atca_derive_key_in_out
{
    uint8_t               mode;          //!< Mode (param 1) of the derive key command
    uint16_t              target_key_id; //!< Key ID (param 2) of the target slot to run the command on
    const uint8_t *       sn;            //!< Device serial number SN[0:8]. Only SN[0:1] and SN[8] are required though.
    const uint8_t *       parent_key;    //!< Parent key to be used in the derive key calculation (32 bytes).
    uint8_t *             target_key;    //!< Derived key will be returned here (32 bytes).
    struct atca_temp_key *temp_key;      //!< Current state of TempKey.
};


/**
 *  \brief Input/output parameters for function atcah_derive_key_mac().
 */
struct atca_derive_key_mac_in_out
{
    uint8_t        mode;            //!< Mode (param 1) of the derive key command
    uint16_t       target_key_id;   //!< Key ID (param 2) of the target slot to run the command on
    const uint8_t *sn;              //!< Device serial number SN[0:8]. Only SN[0:1] and SN[8] are required though.
    const uint8_t *parent_key;      //!< Parent key to be used in the derive key calculation (32 bytes).
    uint8_t *      mac;             //!< DeriveKey MAC will be returned here.
};


/** \struct atca_encrypt_in_out
 *  \brief Input/output parameters for function atca_encrypt().
 *  \var atca_encrypt_in_out::zone
 *       \brief [in] Zone parameter used in Write (Param1).
 *  \var atca_encrypt_in_out::address
 *       \brief [in] Address parameter used in Write command (Param2).
 *  \var atca_encrypt_in_out::crypto_data
 *       \brief [in,out] Pointer to 32-byte data. Input cleartext data, output encrypted data to Write command (Value field).
 *  \var atca_encrypt_in_out::mac
 *       \brief [out] Pointer to 32-byte Mac. Can be set to NULL if input MAC is not required by the Write command (write to OTP, unlocked user zone).
 *  \var atca_encrypt_in_out::temp_key
 *       \brief [in,out] Pointer to TempKey structure.
 */
struct atca_encrypt_in_out
{
    uint8_t               zone;
    uint16_t              address;
    uint8_t *             crypto_data;
    uint8_t *             mac;
    struct atca_temp_key *temp_key;
};


/** \struct atca_decrypt_in_out
 *  \brief Input/output parameters for function atca_decrypt().
 *  \var atca_decrypt_in_out::crypto_data
 *       \brief [in,out] Pointer to 32-byte data. Input encrypted data from Read command (Contents field), output decrypted.
 *  \var atca_decrypt_in_out::temp_key
 *       \brief [in,out] Pointer to TempKey structure.
 */
struct atca_decrypt_in_out
{
    uint8_t *             crypto_data;
    struct atca_temp_key *temp_key;
};


/** \brief Input/output parameters for function atcah_check_mac().
 */
typedef struct atca_check_mac_in_out
{
    uint8_t        mode;            //!< [in] CheckMac command Mode
    uint16_t       key_id;          //!< [in] CheckMac command KeyID
    const uint8_t *sn;              //!< [in] Device serial number SN[0:8]. Only SN[0:1] and SN[8] are required though.
    const uint8_t *client_chal;     //!< [in] ClientChal data, 32 bytes. Can be NULL if mode[0] is 1.
    uint8_t *      client_resp;     //!< [out] Calculated ClientResp will be returned here.
    const uint8_t *other_data;      //!< [in] OtherData, 13 bytes
    const uint8_t *otp;             //!< [in] First 8 bytes of the OTP zone data. Can be NULL is mode[5] is 0.
    const uint8_t *slot_key;        //!< [in] 32 byte key value in the slot specified by slot_id. Can be NULL if mode[1] is 1.
    /// [in] If this is not NULL, it assumes CheckMac copy is enabled for the specified key_id (ReadKey=0). If key_id
    /// is even, this should be the 32-byte key value for the slot key_id+1, otherwise this should be set to slot_key.
    const uint8_t *       target_key;
    struct atca_temp_key *temp_key; //!< [in,out] Current state of TempKey. Required if mode[0] or mode[1] are 1.
} atca_check_mac_in_out_t;


/** \struct atca_verify_in_out
 *  \brief Input/output parameters for function atcah_verify().
 *  \var atca_verify_in_out::curve_type
 *       \brief [in] Curve type used in Verify command (Param2).
 *  \var atca_verify_in_out::signature
 *       \brief [in] Pointer to ECDSA signature to be verified
 *  \var atca_verify_in_out::public_key
 *       \brief [in] Pointer to the public key to be used for verification
 *  \var atca_verify_in_out::temp_key
 *       \brief [in,out] Pointer to TempKey structure.
 */
struct atca_verify_in_out
{
    uint16_t              curve_type;
    const uint8_t *       signature;
    const uint8_t *       public_key;
    struct atca_temp_key *temp_key;
};

/** \brief Input/output parameters for calculating the PubKey digest put into
 *         TempKey by the GenKey command with the
 *         atcah_gen_key_msg() function.
 */
typedef struct atca_gen_key_in_out
{
    uint8_t               mode;            //!< [in] GenKey Mode
    uint16_t              key_id;          //!< [in]  GenKey KeyID
    const uint8_t *       public_key;      //!< [in]  Public key to be used in the PubKey digest. X and Y integers in big-endian format. 64 bytes for P256 curve.
    size_t                public_key_size; //!< [in] Total number of bytes in the public key. 64 bytes for P256 curve.
    const uint8_t *       other_data;      //!< [in]  3 bytes required when bit 4 of the mode is set. Can be NULL otherwise.
    const uint8_t *       sn;              //!< [in] Device serial number SN[0:8] (9 bytes). Only SN[0:1] and SN[8] are required though.
    struct atca_temp_key *temp_key;        //!< [in,out] As input the current state of TempKey. As output, the resulting PubKEy digest.
} atca_gen_key_in_out_t;

/** \brief Input/output parameters for calculating the message and digest used
 *         by the Sign(internal) command. Used with the
 *         atcah_sign_internal_msg() function.
 */
typedef struct atca_sign_internal_in_out
{
    uint8_t                     mode;              //!< [in] Sign Mode
    uint16_t                    key_id;            //!< [in] Sign KeyID
    uint16_t                    slot_config;       //!< [in] SlotConfig[TempKeyFlags.keyId]
    uint16_t                    key_config;        //!< [in] KeyConfig[TempKeyFlags.keyId]
    uint8_t                     use_flag;          //!< [in] UseFlag[TempKeyFlags.keyId], 0x00 for slots 8 and above and for ATECC508A
    uint8_t                     update_count;      //!< [in] UpdateCount[TempKeyFlags.keyId], 0x00 for slots 8 and above and for ATECC508A
    bool                        is_slot_locked;    //!< [in] Is TempKeyFlags.keyId slot locked.
    bool                        for_invalidate;    //!< [in] Set to true if this will be used for the Verify(Invalidate) command.
    const uint8_t *             sn;                //!< [in] Device serial number SN[0:8] (9 bytes)
    const struct atca_temp_key *temp_key;          //!< [in] The current state of TempKey.
    uint8_t*                    message;           //!< [out] Full 55 byte message the Sign(internal) command will build. Can be NULL if not required.
    uint8_t*                    verify_other_data; //!< [out] The 19 byte OtherData bytes to be used with the Verify(In/Validate) command. Can be NULL if not required.
    uint8_t*                    digest;            //!< [out] SHA256 digest of the full 55 byte message. Can be NULL if not required.
} atca_sign_internal_in_out_t;


//! minimum number of bytes in command (from count byte to second CRC byte)
#define ATCA_CMD_SIZE_MIN       ((uint8_t)7)
//! maximum size of command packet (Verify)
#define ATCA_CMD_SIZE_MAX       ((uint8_t)4 * 36 + 7)
//! status byte for success
#define CMD_STATUS_SUCCESS      ((uint8_t)0x00)
//! status byte after wake-up
#define CMD_STATUS_WAKEUP       ((uint8_t)0x11)
//! command parse error
#define CMD_STATUS_BYTE_PARSE   ((uint8_t)0x03)
//! command ECC error
#define CMD_STATUS_BYTE_ECC     ((uint8_t)0x05)
//! command execution error
#define CMD_STATUS_BYTE_EXEC    ((uint8_t)0x0F)
//! communication error
#define CMD_STATUS_BYTE_COMM    ((uint8_t)0xFF)

/** \brief
   @{ */

/** \name opcodes for ATATECC Commands
   @{ */
#define ATCA_CHECKMAC                   ((uint8_t)0x28)         //!< CheckMac command op-code
#define ATCA_DERIVE_KEY                 ((uint8_t)0x1C)         //!< DeriveKey command op-code
#define ATCA_INFO                       ((uint8_t)0x30)         //!< Info command op-code
#define ATCA_GENDIG                     ((uint8_t)0x15)         //!< GenDig command op-code
#define ATCA_GENKEY                     ((uint8_t)0x40)         //!< GenKey command op-code
#define ATCA_HMAC                       ((uint8_t)0x11)         //!< HMAC command op-code
#define ATCA_LOCK                       ((uint8_t)0x17)         //!< Lock command op-code
#define ATCA_MAC                        ((uint8_t)0x08)         //!< MAC command op-code
#define ATCA_NONCE                      ((uint8_t)0x16)         //!< Nonce command op-code
#define ATCA_PAUSE                      ((uint8_t)0x01)         //!< Pause command op-code
#define ATCA_PRIVWRITE                  ((uint8_t)0x46)         //!< PrivWrite command op-code
#define ATCA_RANDOM                     ((uint8_t)0x1B)         //!< Random command op-code
#define ATCA_READ                       ((uint8_t)0x02)         //!< Read command op-code
#define ATCA_SIGN                       ((uint8_t)0x41)         //!< Sign command op-code
#define ATCA_UPDATE_EXTRA               ((uint8_t)0x20)         //!< UpdateExtra command op-code
#define ATCA_VERIFY                     ((uint8_t)0x45)         //!< GenKey command op-code
#define ATCA_WRITE                      ((uint8_t)0x12)         //!< Write command op-code
#define ATCA_ECDH                       ((uint8_t)0x43)         //!< ECDH command op-code
#define ATCA_COUNTER                    ((uint8_t)0x24)         //!< Counter command op-code
#define ATCA_SHA                        ((uint8_t)0x47)         //!< SHA command op-code
/** @} */


/** \name Definitions of Data and Packet Sizes
   @{ */
#define ATCA_BLOCK_SIZE             (32)                                //!< size of a block
#define ATCA_WORD_SIZE              (4)                                 //!< size of a word
#define ATCA_PUB_KEY_PAD            (4)                                 //!< size of the public key pad
#define ATCA_SERIAL_NUM_SIZE        (9)                                 //!< number of bytes in the device serial number
#define ATCA_RSP_SIZE_VAL           ((uint8_t)7)                        //!< size of response packet containing four bytes of data
#define ATCA_KEY_COUNT              (16)                                //!< number of keys
#define ATCA_ECC_CONFIG_SIZE        (128)                               //!< size of configuration zone
#define ATCA_SHA_CONFIG_SIZE        (88)                                //!< size of configuration zone
#define ATCA_OTP_SIZE               (64)                                //!< size of OTP zone
#define ATCA_DATA_SIZE              (ATCA_KEY_COUNT * ATCA_KEY_SIZE)    //!< size of data zone

#define ATCA_COUNT_SIZE             ((uint8_t)1)                        //!< Number of bytes in the command packet Count
#define ATCA_CRC_SIZE               ((uint8_t)2)                        //!< Number of bytes in the command packet CRC
#define ATCA_PACKET_OVERHEAD        (ATCA_COUNT_SIZE + ATCA_CRC_SIZE)   //!< Number of bytes in the command packet

#define ATCA_PUB_KEY_SIZE           (64)                                //!< size of a p256 public key
#define ATCA_PRIV_KEY_SIZE          (32)                                //!< size of a p256 private key
#define ATCA_SIG_SIZE               (64)                                //!< size of a p256 signature
#define RSA2048_KEY_SIZE            (256)                               //!< size of a RSA private key

#define ATCA_RSP_SIZE_MIN           ((uint8_t)4)                        //!< minimum number of bytes in response
#define ATCA_RSP_SIZE_4             ((uint8_t)7)                        //!< size of response packet containing 4 bytes data
#define ATCA_RSP_SIZE_72            ((uint8_t)75)                       //!< size of response packet containing 64 bytes data
#define ATCA_RSP_SIZE_64            ((uint8_t)67)                       //!< size of response packet containing 64 bytes data
#define ATCA_RSP_SIZE_32            ((uint8_t)35)                       //!< size of response packet containing 32 bytes data
#define ATCA_RSP_SIZE_MAX           ((uint8_t)75)                       //!< maximum size of response packet (GenKey and Verify command)

/** \name Definitions for Command Parameter Ranges
   @{ */
#define ATCA_KEY_ID_MAX             ((uint8_t)15)       //!< maximum value for key id
#define ATCA_OTP_BLOCK_MAX          ((uint8_t)1)        //!< maximum value for OTP block
/** @} */

/** \name Definitions for Indexes Common to All Commands
   @{ */
#define ATCA_COUNT_IDX              (0)     //!< command packet index for count
#define ATCA_OPCODE_IDX             (1)     //!< command packet index for op-code
#define ATCA_PARAM1_IDX             (2)     //!< command packet index for first parameter
#define ATCA_PARAM2_IDX             (3)     //!< command packet index for second parameter
#define ATCA_DATA_IDX               (5)     //!< command packet index for data load
#define ATCA_RSP_DATA_IDX           (1)     //!< buffer index of data in response
/** @} */

/** \name Definitions for Zone and Address Parameters
   @{ */
#define ATCA_ZONE_CONFIG                ((uint8_t)0x00)         //!< Configuration zone
#define ATCA_ZONE_OTP                   ((uint8_t)0x01)         //!< OTP (One Time Programming) zone
#define ATCA_ZONE_DATA                  ((uint8_t)0x02)         //!< Data zone
#define ATCA_ZONE_MASK                  ((uint8_t)0x03)         //!< Zone mask
#define ATCA_ZONE_ENCRYPTED             ((uint8_t)0x40)         //!< Zone bit 6 set: Write is encrypted with an unlocked data zone.
#define ATCA_ZONE_READWRITE_32          ((uint8_t)0x80)         //!< Zone bit 7 set: Access 32 bytes, otherwise 4 bytes.
#define ATCA_ADDRESS_MASK_CONFIG        (0x001F)                //!< Address bits 5 to 7 are 0 for Configuration zone.
#define ATCA_ADDRESS_MASK_OTP           (0x000F)                //!< Address bits 4 to 7 are 0 for OTP zone.
#define ATCA_ADDRESS_MASK               (0x007F)                //!< Address bit 7 to 15 are always 0.
/** @} */

/** \name Definitions for ECC Key type
   @{ */
#define ATCA_B283_KEY_TYPE          0       //!< B283 NIST ECC key
#define ATCA_K283_KEY_TYPE          1       //!< K283 NIST ECC key
#define ATCA_P256_KEY_TYPE          4       //!< P256 NIST ECC key
/** @} */

/** \name Definitions for the ECDH Command
   @{ */
#define ECDH_PREFIX_MODE                    ((uint8_t)0x00)
#define ECDH_COUNT                          (71)
#define ECDH_PUBKEYIN_SIZE                  (64)
/** @} */

/** \name Definitions for the COUNTER Command
   @{ */
#define COUNTER_COUNT                       ATCA_CMD_SIZE_MIN
#define COUNTER_MODE_IDX                    ATCA_PARAM1_IDX         //!< COUNTER command index for mode
#define COUNTER_KEYID_IDX                   ATCA_PARAM2_IDX         //!< COUNTER command index for key id
#define COUNTER_CHALLENGE_IDX               ATCA_DATA_IDX           //!< COUNTER command index for optional challenge
#define COUNTER_COUNT_LONG                  (70)                    //!< COUNTER command packet size without challenge
#define COUNTER_MODE_MASK                   ((uint8_t)0x01)         //!< COUNTER mode bits 1 to 7 are 0
typedef enum
{
    COUNTER_MODE_READ = 0,
    COUNTER_MODE_INCREASE = 1
} enum_counter_mode;
/** @} */

/** \name Definitions for the SHA Command
   @{ */
#define SHA_COUNT_SHORT                     ATCA_CMD_SIZE_MIN
#define SHA_COUNT_LONG                      (7)
#define ATCA_SHA_DIGEST_SIZE                        (32)
#define SHA_DATA_MAX                        (64)
#define ATCA_SHA256_BLOCK_SIZE              (64)

#define SHA_MODE_SHA256_START               ((uint8_t)0x00) //!< Initialization, does not accept a message
#define SHA_MODE_SHA256_UPDATE              ((uint8_t)0x01) //!< Add 64 bytes in the meesage to the SHA context
#define SHA_MODE_SHA256_END                 ((uint8_t)0x02) //!< Complete the calculation and return the digest
#define SHA_MODE_SHA256_PUBLIC              ((uint8_t)0x03) //!< Add 64 byte ECC public key in the slot to the SHA context
#define SHA_MODE_HMAC_START                 ((uint8_t)0x04) //!< Initialization, HMAC calculation
#define SHA_MODE_HMAC_UPDATE                ((uint8_t)0x01) //!< Add 64 bytes in the meesage to the SHA context
#define SHA_MODE_HMAC_END                   ((uint8_t)0x05) //!< Complete the HMAC computation and return digest
/** @} */


/** \name Definitions for the CheckMac Command
   @{ */
#define CHECKMAC_MODE_IDX                   ATCA_PARAM1_IDX     //!< CheckMAC command index for mode
#define CHECKMAC_KEYID_IDX                  ATCA_PARAM2_IDX     //!< CheckMAC command index for key identifier
#define CHECKMAC_CLIENT_CHALLENGE_IDX       ATCA_DATA_IDX       //!< CheckMAC command index for client challenge
#define CHECKMAC_CLIENT_RESPONSE_IDX        (37)                //!< CheckMAC command index for client response
#define CHECKMAC_DATA_IDX                   (69)                //!< CheckMAC command index for other data
#define CHECKMAC_COUNT                      (84)                //!< CheckMAC command packet size
#define CHECKMAC_MODE_CHALLENGE             ((uint8_t)0x00)     //!< CheckMAC mode	   0: first SHA block from key id
#define CHECKMAC_MODE_BLOCK2_TEMPKEY        ((uint8_t)0x01)     //!< CheckMAC mode bit   0: second SHA block from TempKey
#define CHECKMAC_MODE_BLOCK1_TEMPKEY        ((uint8_t)0x02)     //!< CheckMAC mode bit   1: first SHA block from TempKey
#define CHECKMAC_MODE_SOURCE_FLAG_MATCH     ((uint8_t)0x04)     //!< CheckMAC mode bit   2: match TempKey.SourceFlag
#define CHECKMAC_MODE_INCLUDE_OTP_64        ((uint8_t)0x20)     //!< CheckMAC mode bit   5: include first 64 OTP bits
#define CHECKMAC_MODE_MASK                  ((uint8_t)0x27)     //!< CheckMAC mode bits 3, 4, 6, and 7 are 0.
#define CHECKMAC_CLIENT_CHALLENGE_SIZE      (32)                //!< CheckMAC size of client challenge
#define CHECKMAC_CLIENT_RESPONSE_SIZE       (32)                //!< CheckMAC size of client response
#define CHECKMAC_OTHER_DATA_SIZE            (13)                //!< CheckMAC size of "other data"
#define CHECKMAC_CLIENT_COMMAND_SIZE        (4)                 //!< CheckMAC size of client command header size inside "other data"
#define CHECKMAC_CMD_MATCH                  (0)                 //!< CheckMAC return value when there is a match
#define CHECKMAC_CMD_MISMATCH               (1)                 //!< CheckMAC return value when there is a mismatch
/** @} */

/** \name Definitions for the DeriveKey Command
   @{ */
#define DERIVE_KEY_RANDOM_IDX           ATCA_PARAM1_IDX     //!< DeriveKey command index for random bit
#define DERIVE_KEY_TARGETKEY_IDX        ATCA_PARAM2_IDX     //!< DeriveKey command index for target slot
#define DERIVE_KEY_MAC_IDX              ATCA_DATA_IDX       //!< DeriveKey command index for optional MAC
#define DERIVE_KEY_COUNT_SMALL          ATCA_CMD_SIZE_MIN   //!< DeriveKey command packet size without MAC
#define DERIVE_KEY_MODE                 ((uint8_t)0x04)     //!< DeriveKey command mode set to 4 as in datasheet
#define DERIVE_KEY_COUNT_LARGE          (39)                //!< DeriveKey command packet size with MAC
#define DERIVE_KEY_RANDOM_FLAG          ((uint8_t)4)        //!< DeriveKey 1. parameter; has to match TempKey.SourceFlag
#define DERIVE_KEY_MAC_SIZE             (32)                //!< DeriveKey MAC size
/** @} */

/** \name Definitions for the GenDig Command
   @{ */
#define GENDIG_ZONE_IDX             ATCA_PARAM1_IDX     //!< GenDig command index for zone
#define GENDIG_KEYID_IDX            ATCA_PARAM2_IDX     //!< GenDig command index for key id
#define GENDIG_DATA_IDX             ATCA_DATA_IDX       //!< GenDig command index for optional data
#define GENDIG_COUNT                ATCA_CMD_SIZE_MIN   //!< GenDig command packet size without "other data"
#define GENDIG_ZONE_CONFIG          ((uint8_t)0)        //!< GenDig zone id config. Use KeyID to specify any of the four 256-bit blocks of the Configuration zone.
#define GENDIG_ZONE_OTP             ((uint8_t)1)        //!< GenDig zone id OTP. Use KeyID to specify either the first or second 256-bit block of the OTP zone.
#define GENDIG_ZONE_DATA            ((uint8_t)2)        //!< GenDig zone id data. Use KeyID to specify a slot in the Data zone or a transport key in the hardware array.
#define GENDIG_ZONE_SHARED_NONCE    ((uint8_t)3)        //!< GenDig zone id shared nonce. KeyID specifies the location of the input value in the message generation.
#define GENDIG_ZONE_COUNTER         ((uint8_t)4)        //!< GenDig zone id counter. KeyID specifies the monotonic counter ID to be included in the message generation.
#define GENDIG_ZONE_KEY_CONFIG      ((uint8_t)5)        //!< GenDig zone id key config. KeyID specifies the slot for which the configuration information is to be included in the message generation.
/** @} */

/** \name Definitions for the GenKey Command
   @{ */
#define GENKEY_MODE_IDX             ATCA_PARAM1_IDX         //!< GenKey command index for mode
#define GENKEY_KEYID_IDX            ATCA_PARAM2_IDX         //!< GenKey command index for key id
#define GENKEY_DATA_IDX             (5)                     //!< GenKey command index for other data
#define GENKEY_COUNT                ATCA_CMD_SIZE_MIN       //!< GenKey command packet size without "other data"
#define GENKEY_COUNT_DATA           (10)                    //!< GenKey command packet size with "other data"
#define GENKEY_OTHER_DATA_SIZE      (3)                     //!< GenKey size of "other data"
#define GENKEY_MODE_MASK            ((uint8_t)0x1C)         //!< GenKey mode bits 0 to 1 and 5 to 7 are 0
#define GENKEY_MODE_PRIVATE         ((uint8_t)0x04)         //!< GenKey mode: private key generation
#define GENKEY_MODE_PUBLIC          ((uint8_t)0x00)         //!< GenKey mode: public key calculation
#define GENKEY_MODE_DIGEST          ((uint8_t)0x08)         //!< GenKey mode: PubKey digest will be created after the public key is calculated
#define GENKEY_MODE_PUBKEY_DIGEST   ((uint8_t)0x10)         //!< GenKey mode: Calculate PubKey digest on the public key in KeyId
/** @} */
/** \name Definitions for the GENKEY Command
   @{ */
/** @} */


/** \name Definitions for the HMAC Command
   @{ */
#define HMAC_MODE_IDX               ATCA_PARAM1_IDX     //!< HMAC command index for mode
#define HMAC_KEYID_IDX              ATCA_PARAM2_IDX     //!< HMAC command index for key id
#define HMAC_COUNT                  ATCA_CMD_SIZE_MIN   //!< HMAC command packet size
#define HMAC_MODE_FLAG_TK_RAND      ((uint8_t)0x00)     //!< HMAC mode bit 2: The value of this bit must match the value in TempKey.SourceFlag or the command will return an error.
#define HMAC_MODE_FLAG_TK_NORAND    ((uint8_t)0x04)     //!< HMAC mode bit 2: The value of this bit must match the value in TempKey.SourceFlag or the command will return an error.
#define HMAC_MODE_FLAG_OTP88        ((uint8_t)0x10)     //!< HMAC mode bit 4: Include the first 88 OTP bits (OTP[0] through OTP[10]) in the message.; otherwise, the corresponding message bits are set to zero. Not applicable for ATECC508A.
#define HMAC_MODE_FLAG_OTP64        ((uint8_t)0x20)     //!< HMAC mode bit 5: Include the first 64 OTP bits (OTP[0] through OTP[7]) in the message.; otherwise, the corresponding message bits are set to zero. If Mode[4] is set, the value of this mode bit is ignored. Not applicable for ATECC508A.
#define HMAC_MODE_FLAG_FULLSN       ((uint8_t)0x40)     //!< HMAC mode bit 6: If set, include the 48 bits SN[2:3] and SN[4:7] in the message.; otherwise, the corresponding message bits are set to zero.
#define HMAC_MODE_MASK              ((uint8_t)0x74)     //!< HMAC mode bits 0, 1, 3, and 7 are 0.
#define HMAC_DIGEST_SIZE            (32)                //!< HMAC size of digest response
/** @} */

/** \name Definitions for the Info Command
   @{ */
#define INFO_PARAM1_IDX             ATCA_PARAM1_IDX     //!< Info command index for 1. parameter
#define INFO_PARAM2_IDX             ATCA_PARAM2_IDX     //!< Info command index for 2. parameter
#define INFO_COUNT                  ATCA_CMD_SIZE_MIN   //!< Info command packet size
#define INFO_MODE_REVISION          ((uint8_t)0x00)     //!< Info mode Revision
#define INFO_MODE_KEY_VALID         ((uint8_t)0x01)     //!< Info mode KeyValid
#define INFO_MODE_STATE             ((uint8_t)0x02)     //!< Info mode State
#define INFO_MODE_GPIO              ((uint8_t)0x03)     //!< Info mode GPIO
#define INFO_MODE_MAX               ((uint8_t)0x03)     //!< Info mode maximum value
#define INFO_NO_STATE               ((uint8_t)0x00)     //!< Info mode is not the state mode.
#define INFO_OUTPUT_STATE_MASK      ((uint8_t)0x01)     //!< Info output state mask
#define INFO_DRIVER_STATE_MASK      ((uint8_t)0x02)     //!< Info driver state mask
#define INFO_PARAM2_MAX             ((uint8_t)0x03)     //!< Info param2 (state) maximum value
#define INFO_SIZE                   ((uint8_t)0x04)     //!< Info param2 (state) maximum value
/** @} */

/** \name Definitions for the Lock Command
   @{ */
#define LOCK_ZONE_IDX               ATCA_PARAM1_IDX     //!< Lock command index for zone
#define LOCK_SUMMARY_IDX            ATCA_PARAM2_IDX     //!< Lock command index for summary
#define LOCK_COUNT                  ATCA_CMD_SIZE_MIN   //!< Lock command packet size
#define LOCK_ZONE_CONFIG            ((uint8_t)0x00)     //!< Lock zone is Config
#define LOCK_ZONE_DATA              ((uint8_t)0x01)     //!< Lock zone is OTP or Data
#define LOCK_ZONE_DATA_SLOT         ((uint8_t)0x02)     //!< Lock slot of Data
#define LOCK_ZONE_NO_CRC            ((uint8_t)0x80)     //!< Lock command: Ignore summary.
#define LOCK_ZONE_MASK              (0xBF)              //!< Lock parameter 1 bits 6 are 0.
#define ATCA_UNLOCKED               (0x55)              //!< Value indicating an unlocked zone
#define ATCA_LOCKED                 (0x00)              //!< Value indicating a locked zone
/** @} */

/** \name Definitions for the MAC Command
   @{ */
#define MAC_MODE_IDX                    ATCA_PARAM1_IDX         //!< MAC command index for mode
#define MAC_KEYID_IDX                   ATCA_PARAM2_IDX         //!< MAC command index for key id
#define MAC_CHALLENGE_IDX               ATCA_DATA_IDX           //!< MAC command index for optional challenge
#define MAC_COUNT_SHORT                 ATCA_CMD_SIZE_MIN       //!< MAC command packet size without challenge
#define MAC_COUNT_LONG                  (39)                    //!< MAC command packet size with challenge
#define MAC_MODE_CHALLENGE              ((uint8_t)0x00)         //!< MAC mode       0: first SHA block from data slot
#define MAC_MODE_BLOCK2_TEMPKEY         ((uint8_t)0x01)         //!< MAC mode bit   0: second SHA block from TempKey
#define MAC_MODE_BLOCK1_TEMPKEY         ((uint8_t)0x02)         //!< MAC mode bit   1: first SHA block from TempKey
#define MAC_MODE_SOURCE_FLAG_MATCH      ((uint8_t)0x04)         //!< MAC mode bit   2: match TempKey.SourceFlag
#define MAC_MODE_PTNONCE_TEMPKEY        ((uint8_t)0x06)         //!< MAC mode bit   0: second SHA block from TempKey
#define MAC_MODE_PASSTHROUGH            ((uint8_t)0x07)         //!< MAC mode bit 0-2: pass-through mode
#define MAC_MODE_INCLUDE_OTP_88         ((uint8_t)0x10)         //!< MAC mode bit   4: include first 88 OTP bits
#define MAC_MODE_INCLUDE_OTP_64         ((uint8_t)0x20)         //!< MAC mode bit   5: include first 64 OTP bits
#define MAC_MODE_INCLUDE_SN             ((uint8_t)0x40)         //!< MAC mode bit   6: include serial number
#define MAC_CHALLENGE_SIZE              (32)                    //!< MAC size of challenge
#define MAC_SIZE                        (32)                    //!< MAC size of response
#define MAC_MODE_MASK                   ((uint8_t)0x77)         //!< MAC mode bits 3 and 7 are 0.
/** @} */

/** \name Definitions for the Nonce Command
   @{ */
#define NONCE_MODE_IDX                  ATCA_PARAM1_IDX     //!< Nonce command index for mode
#define NONCE_PARAM2_IDX                ATCA_PARAM2_IDX     //!< Nonce command index for 2. parameter
#define NONCE_INPUT_IDX                 ATCA_DATA_IDX       //!< Nonce command index for input data
#define NONCE_COUNT_SHORT               (27)                //!< Nonce command packet size for 20 bytes of data
#define NONCE_COUNT_LONG                (39)                //!< Nonce command packet size for 32 bytes of data
#define NONCE_MODE_MASK                 ((uint8_t)0x03)     //!< Nonce mode bits 2 to 7 are 0.
#define NONCE_MODE_SEED_UPDATE          ((uint8_t)0x00)     //!< Nonce mode: update seed
#define NONCE_MODE_NO_SEED_UPDATE       ((uint8_t)0x01)     //!< Nonce mode: do not update seed
#define NONCE_MODE_INVALID              ((uint8_t)0x02)     //!< Nonce mode 2 is invalid.
#define NONCE_MODE_PASSTHROUGH          ((uint8_t)0x03)     //!< Nonce mode: pass-through
#define NONCE_MODE_RANDOM_OUT           ((uint16_t)0x0000)  //!< Nonce mode: output RandOut or single byte of zero
#define NONCE_MODE_TEMPKEY_OUT          ((uint16_t)0x0080)  //!< Nonce mode: output RandOut or single byte of zero
#define NONCE_NUMIN_SIZE                (20)                //!< Nonce data length
#define NONCE_NUMIN_SIZE_PASSTHROUGH    (32)                //!< Nonce data length in pass-through mode (mode = 3)
/** @} */

/** \name Definitions for the Pause Command
   @{ */
#define PAUSE_SELECT_IDX                ATCA_PARAM1_IDX     //!< Pause command index for Selector
#define PAUSE_PARAM2_IDX                ATCA_PARAM2_IDX     //!< Pause command index for 2. parameter
#define PAUSE_COUNT                     ATCA_CMD_SIZE_MIN   //!< Pause command packet size
/** @} */

/** \name Definitions for the PrivWrite Command
   @{ */
#define PRIVWRITE_ZONE_IDX          ATCA_PARAM1_IDX     //!< PrivWrite command index for zone
#define PRIVWRITE_KEYID_IDX         ATCA_PARAM2_IDX     //!< PrivWrite command index for KeyID
#define PRIVWRITE_VALUE_IDX         ( 5)                //!< PrivWrite command index for value
#define PRIVWRITE_MAC_IDX           (41)                //!< PrivWrite command index for MAC
#define PRIVWRITE_COUNT             (75)                //!< PrivWrite command packet size
#define PRIVWRITE_ZONE_MASK         ((uint8_t)0x40)     //!< PrivWrite zone bits 0 to 5 and 7 are 0.
#define PRIVWRITE_MODE_ENCRYPT      ((uint8_t)0x40)     //!< PrivWrite mode: encrypted
/** @} */

/** \name Definitions for the Random Command
   @{ */
#define RANDOM_MODE_IDX                 ATCA_PARAM1_IDX     //!< Random command index for mode
#define RANDOM_PARAM2_IDX               ATCA_PARAM2_IDX     //!< Random command index for 2. parameter
#define RANDOM_COUNT                    ATCA_CMD_SIZE_MIN   //!< Random command packet size
#define RANDOM_SEED_UPDATE              ((uint8_t)0x00)     //!< Random mode for automatic seed update
#define RANDOM_NO_SEED_UPDATE           ((uint8_t)0x01)     //!< Random mode for no seed update
#define RANDOM_NUM_SIZE                 ((uint8_t)0x20)     //!< Number of bytes in the data packet of a random command
/** @} */

/** \name Definitions for the Read Command
   @{ */
#define READ_ZONE_IDX               ATCA_PARAM1_IDX         //!< Read command index for zone
#define READ_ADDR_IDX               ATCA_PARAM2_IDX         //!< Read command index for address
#define READ_COUNT                  ATCA_CMD_SIZE_MIN       //!< Read command packet size
#define READ_ZONE_MASK              ((uint8_t)0x83)         //!< Read zone bits 2 to 6 are 0.
/** @} */

/** \name Definitions for the Sign Command
   @{ */
#define SIGN_MODE_IDX               ATCA_PARAM1_IDX     //!< Sign command index for mode
#define SIGN_KEYID_IDX              ATCA_PARAM2_IDX     //!< Sign command index for key id
#define SIGN_COUNT                  ATCA_CMD_SIZE_MIN   //!< Sign command packet size
#define SIGN_MODE_MASK              ((uint8_t)0xC0)     //!< Sign mode bits 0 to 5 are 0
#define SIGN_MODE_INTERNAL          ((uint8_t)0x00)     //!< Sign mode	 0: internal
#define SIGN_MODE_INVALIDATE        ((uint8_t)0x01)     //!< Sign mode bit 1: Signature will be used for Verify(Invalidate)
#define SIGN_MODE_INCLUDE_SN        ((uint8_t)0x40)     //!< Sign mode bit 6: include serial number
#define SIGN_MODE_EXTERNAL          ((uint8_t)0x80)     //!< Sign mode bit 7: external
/** @} */

/** \name Definitions for the UpdateExtra Command
   @{ */
#define UPDATE_MODE_IDX             ATCA_PARAM1_IDX     //!< UpdateExtra command index for mode
#define UPDATE_VALUE_IDX            ATCA_PARAM2_IDX     //!< UpdateExtra command index for new value
#define UPDATE_COUNT                ATCA_CMD_SIZE_MIN   //!< UpdateExtra command packet size
#define UPDATE_MODE_USER_EXTRA      ((uint8_t)0x00)     //!< UpdateExtra mode: update Config byte 84 (user extra)
#define UPDATE_MODE_SELECTOR        ((uint8_t)0x01)     //!< UpdateExtra mode: update Config byte 85 (selector)
#define UPDATE_MODE_DEC_COUNTER     ((uint8_t)0x02)     //!< UpdateExtra mode: decrement counter
/** @} */

/** \name Definitions for the Verify Command
   @{ */
#define VERIFY_MODE_IDX             ATCA_PARAM1_IDX         //!< Verify command index for mode
#define VERIFY_KEYID_IDX            ATCA_PARAM2_IDX         //!< Verify command index for key id
#define VERIFY_DATA_IDX             (  5)                   //!< Verify command index for data
#define VERIFY_256_STORED_COUNT     ( 71)                   //!< Verify command packet size for 256-bit key in stored mode
#define VERIFY_283_STORED_COUNT     ( 79)                   //!< Verify command packet size for 283-bit key in stored mode
#define VERIFY_256_VALIDATE_COUNT   ( 90)                   //!< Verify command packet size for 256-bit key in validate mode
#define VERIFY_283_VALIDATE_COUNT   ( 98)                   //!< Verify command packet size for 283-bit key in validate mode
#define VERIFY_256_EXTERNAL_COUNT   (135)                   //!< Verify command packet size for 256-bit key in external mode
#define VERIFY_283_EXTERNAL_COUNT   (151)                   //!< Verify command packet size for 283-bit key in external mode
#define VERIFY_256_KEY_SIZE         ( 64)                   //!< Verify key size for 256-bit key
#define VERIFY_283_KEY_SIZE         ( 72)                   //!< Verify key size for 283-bit key
#define VERIFY_256_SIGNATURE_SIZE   ( 64)                   //!< Verify signature size for 256-bit key
#define VERIFY_283_SIGNATURE_SIZE   ( 72)                   //!< Verify signature size for 283-bit key
#define VERIFY_OTHER_DATA_SIZE      ( 19)                   //!< Verify size of "other data"
#define VERIFY_MODE_MASK            ((uint8_t)0x03)         //!< Verify mode bits 2 to 7 are 0
#define VERIFY_MODE_STORED          ((uint8_t)0x00)         //!< Verify mode: stored
#define VERIFY_MODE_VALIDATE_EXTERNAL  ((uint8_t)0x01)      //!< Verify mode: validate external
#define VERIFY_MODE_EXTERNAL        ((uint8_t)0x02)         //!< Verify mode: external
#define VERIFY_MODE_VALIDATE        ((uint8_t)0x03)         //!< Verify mode: validate
#define VERIFY_MODE_INVALIDATE      ((uint8_t)0x07)         //!< Verify mode: invalidate
#define VERIFY_KEY_B283             ((uint16_t)0x0000)      //!< Verify key type: B283
#define VERIFY_KEY_K283             ((uint16_t)0x0001)      //!< Verify key type: K283
#define VERIFY_KEY_P256             ((uint16_t)0x0004)      //!< Verify key type: P256
/** @} */

/** \name Definitions for the Write Command
   @{ */
#define WRITE_ZONE_IDX              ATCA_PARAM1_IDX     //!< Write command index for zone
#define WRITE_ADDR_IDX              ATCA_PARAM2_IDX     //!< Write command index for address
#define WRITE_VALUE_IDX             ATCA_DATA_IDX       //!< Write command index for data
#define WRITE_MAC_VS_IDX            ( 9)                //!< Write command index for MAC following short data
#define WRITE_MAC_VL_IDX            (37)                //!< Write command index for MAC following long data
#define WRITE_MAC_SIZE              (32)                //!< Write MAC size
#define WRITE_ZONE_MASK             ((uint8_t)0xC3)     //!< Write zone bits 2 to 5 are 0.
#define WRITE_ZONE_WITH_MAC         ((uint8_t)0x40)     //!< Write zone bit 6: write encrypted with MAC
#define WRITE_ZONE_OTP              ((uint8_t)1)        //!< WRITE zone id OTP
#define WRITE_ZONE_DATA             ((uint8_t)2)        //!< WRITE zone id data
/** @} */

/** \name Response Size Definitions
   @{ */
#define CHECKMAC_RSP_SIZE           ATCA_RSP_SIZE_MIN   //!< response size of DeriveKey command
#define DERIVE_KEY_RSP_SIZE         ATCA_RSP_SIZE_MIN   //!< response size of DeriveKey command
#define GENDIG_RSP_SIZE             ATCA_RSP_SIZE_MIN   //!< response size of GenDig command
#define GENKEY_RSP_SIZE_SHORT       ATCA_RSP_SIZE_MIN   //!< response size of GenKey command in Digest mode
#define GENKEY_RSP_SIZE_LONG        ATCA_RSP_SIZE_72    //!< response size of GenKey command when generating key
#define HMAC_RSP_SIZE               ATCA_RSP_SIZE_32    //!< response size of HMAC command
#define INFO_RSP_SIZE               ATCA_RSP_SIZE_VAL   //!< response size of Info command returns 4 bytes
#define LOCK_RSP_SIZE               ATCA_RSP_SIZE_MIN   //!< response size of Lock command
#define MAC_RSP_SIZE                ATCA_RSP_SIZE_32    //!< response size of MAC command
#define NONCE_RSP_SIZE_SHORT        ATCA_RSP_SIZE_MIN   //!< response size of Nonce command with mode[0:1] = 3
#define NONCE_RSP_SIZE_LONG         ATCA_RSP_SIZE_32    //!< response size of Nonce command
#define PAUSE_RSP_SIZE              ATCA_RSP_SIZE_MIN   //!< response size of Pause command
#define PRIVWRITE_RSP_SIZE          ATCA_RSP_SIZE_MIN   //!< response size of PrivWrite command
#define RANDOM_RSP_SIZE             ATCA_RSP_SIZE_32    //!< response size of Random command
#define READ_4_RSP_SIZE             ATCA_RSP_SIZE_VAL   //!< response size of Read command when reading 4 bytes
#define READ_32_RSP_SIZE            ATCA_RSP_SIZE_32    //!< response size of Read command when reading 32 bytes
#define SIGN_RSP_SIZE               ATCA_RSP_SIZE_MAX   //!< response size of Sign command
#define SHA_RSP_SIZE                ATCA_RSP_SIZE_32    //!< response size of SHA command
#define UPDATE_RSP_SIZE             ATCA_RSP_SIZE_MIN   //!< response size of UpdateExtra command
#define VERIFY_RSP_SIZE             ATCA_RSP_SIZE_MIN   //!< response size of UpdateExtra command
#define WRITE_RSP_SIZE              ATCA_RSP_SIZE_MIN   //!< response size of Write command

#define ECDH_KEY_SIZE               ATCA_BLOCK_SIZE     //!< response size of ECDH command
#define ECDH_RSP_SIZE               ATCA_RSP_SIZE_32    //!< response size of ECDH command
#define COUNTER_RSP_SIZE            ATCA_RSP_SIZE_4     //!< response size of COUNTER command
#define SHA_RSP_SIZE_SHORT          ATCA_RSP_SIZE_MIN   //!< response size of SHA command
#define SHA_RSP_SIZE_LONG           ATCA_RSP_SIZE_32    //!< response size of SHA command


typedef enum
{
    ATCA_SUCCESS                = 0x00, //!< Function succeeded.
    ATCA_CONFIG_ZONE_LOCKED     = 0x01,
    ATCA_DATA_ZONE_LOCKED       = 0x02,
    ATCA_WAKE_FAILED            = 0xD0, //!< response status byte indicates CheckMac failure (status byte = 0x01)
    ATCA_CHECKMAC_VERIFY_FAILED = 0xD1, //!< response status byte indicates CheckMac failure (status byte = 0x01)
    ATCA_PARSE_ERROR            = 0xD2, //!< response status byte indicates parsing error (status byte = 0x03)
    ATCA_STATUS_CRC             = 0xD4, //!< response status byte indicates CRC error (status byte = 0xFF)
    ATCA_STATUS_UNKNOWN         = 0xD5, //!< response status byte is unknown
    ATCA_STATUS_ECC             = 0xD6, //!< response status byte is ECC fault (status byte = 0x05)
    ATCA_FUNC_FAIL              = 0xE0, //!< Function could not execute due to incorrect condition / state.
    ATCA_GEN_FAIL               = 0xE1, //!< unspecified error
    ATCA_BAD_PARAM              = 0xE2, //!< bad argument (out of range, null pointer, etc.)
    ATCA_INVALID_ID             = 0xE3, //!< invalid device id, id not set
    ATCA_INVALID_SIZE           = 0xE4, //!< Count value is out of range or greater than buffer size.
    ATCA_BAD_CRC                = 0xE5, //!< incorrect CRC received
    ATCA_RX_FAIL                = 0xE6, //!< Timed out while waiting for response. Number of bytes received is > 0.
    ATCA_RX_NO_RESPONSE         = 0xE7, //!< Not an error while the Command layer is polling for a command response.
    ATCA_RESYNC_WITH_WAKEUP     = 0xE8, //!< Re-synchronization succeeded, but only after generating a Wake-up
    ATCA_PARITY_ERROR           = 0xE9, //!< for protocols needing parity
    ATCA_TX_TIMEOUT             = 0xEA, //!< for Atmel PHY protocol, timeout on transmission waiting for master
    ATCA_RX_TIMEOUT             = 0xEB, //!< for Atmel PHY protocol, timeout on receipt waiting for master
    ATCA_COMM_FAIL              = 0xF0, //!< Communication with device failed. Same as in hardware dependent modules.
    ATCA_TIMEOUT                = 0xF1, //!< Timed out while waiting for response. Number of bytes received is 0.
    ATCA_BAD_OPCODE             = 0xF2, //!< opcode is not supported by the device
    ATCA_WAKE_SUCCESS           = 0xF3, //!< received proper wake token
    ATCA_EXECUTION_ERROR        = 0xF4, //!< chip was in a state where it could not execute the command, response status byte indicates command execution error (status byte = 0x0F)
    ATCA_UNIMPLEMENTED          = 0xF5, //!< Function or some element of it hasn't been implemented yet
    ATCA_ASSERT_FAILURE         = 0xF6, //!< Code failed run-time consistency check
    ATCA_TX_FAIL                = 0xF7, //!< Failed to write
    ATCA_NOT_LOCKED             = 0xF8, //!< required zone was not locked
    ATCA_NO_DEVICES             = 0xF9, //!< For protocols that support device discovery (kit protocol), no devices were found
    ATCA_NOT_INIT               = 0xFA, //!< Indication that library or context was not initialized prior to an API call
} ATCA_STATUS;

#endif // SHA2_ROUTINES_H