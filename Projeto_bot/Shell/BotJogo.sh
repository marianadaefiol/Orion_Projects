#!/bin/sh

export DISPLAY=:0

#Adicionar 5000 créditos
xdotool keydown c
xdotool keyup c
xdotool keydown c
xdotool keyup c
xdotool keydown c
xdotool keyup c
xdotool keydown c
xdotool keyup c
xdotool keydown c
xdotool keyup c
#Inicializa Contadores para algumas ações
#Forçar jogadas com 20 Linhas
i=0
#Inserção de créditos
j=0
#Reservado (Caso entre em pagamento automático)
k=0

#Loop While que fica realizando as jogadas e demais ações desejadas
while : 
do
  #Realizada as jogadas
  xdotool keydown 0
  xdotool keyup 0

  #Incrementa os contadores
  i=$(($i + 1))
  j=$(($j + 1))
  k=$(($k + 1))
  sleep .1

  #Realizar a ação do botão de 20 linhas
  if [ $i -eq 20 ];
  then
    xdotool keydown 5
    xdotool keydown 5
    i=0
    sleep .1
  fi

  #Inserir mais 5000 créditos
  if [ $j -eq 80 ];
  then
    xdotool keydown c
    xdotool keyup c
    xdotool keydown c
    xdotool keyup c
    xdotool keydown c
    xdotool keyup c
    xdotool keydown c
    xdotool keyup c
    xdotool keydown c
    xdotool keyup c
    j=0
    sleep .1
  fi

  #Pressiona o botão de reservado para sair de qualquer eventual pagamento
  if [ $k -eq 200 ];
  then
    xdotool keydown 8
    xdotool keyup 8
    k=0
    sleep .1
  fi
done

