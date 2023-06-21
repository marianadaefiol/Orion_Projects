
Esse post tem o objetivo de apresentar um passo a passo para a configuração básica de um ambiente de desenvolvimento C++ no Ubuntu 22.04.

Passo 1: Instalação do Sistema Operacional

Caso a máquina já esteja com o Ubuntu 22.04 instalado, esse passo pode ser pulado. Mas caso não esteja, será necessário fazer a formatação da máquina e instalar o ubuntu. Para isso, primeiramente faça o backup dos arquivos importantes e suba a máquina a partir de um pendrive bootável. Caso o sistema operacional atual seja uma versão anterior do ubuntu, existe a possibilidade de fazer o upgrade de versão, mas para garantir que não haja nenhum conflito é preferível que se faça a formatação.

Esse tutorial pode ser utilizado para um ubuntu nativo ou para uma maquina virtual.

1 - Maquina Virtual: VirtualBox 

.1 - O download pode ser feito atraves do link: https://www.virtualbox.org/wiki/Downloads. 
.2 - No link, em "VirtualBox 7.0.8 platform packages", selecione "​Windows hosts" para fazer o download. 
.3 - Após completar o dowload, faça a instalação e inicie o aplicativo. 

2 - Download ISO Ubuntu 22.04 Acessar o site oficial do Ubuntu

.1 - Abra o seu navegador de internet e vá para o site oficial do Ubuntu em https://ubuntu.com/.
.2 - Clique na seção "Desktop".
.3 - Na página do Ubuntu Desktop, você verá a versão mais recente disponível. Role a página até encontrar o botão "Download" abaixo da versão do Ubuntu 22.04.
.4 - Clique no botão "Download" para iniciar o download da imagem ISO.

3 -  Configurando o VirtualBox

.1 - Abra o VirtualBox.
.2 - Clique no botão "Novo" para criar uma nova máquina virtual.
.3 - Na janela de criação da nova máquina virtual, digite um nome para a máquina (por exemplo, "Ubuntu Orion") e selecione a imagem ISO do Ubuntu 22.04 que foi realizado download anteriormente.
.4 - Selecione o Tipo "Linux", a Versão "Ubuntu 22.04 LTS (64-bit)" e selecione o botão "Próximo". 
.5 - Escolha a quantidade de memória RAM que deseja alocar (recomendável pelo menos 4GB) para a máquina virtual e clique em "Próximo".
.6 - Em Processadores, selecione o suficiente para não atingir a faixa laranja e clique em "Próximo". 
.7 - Selecione o campo "Criar um novo disco rígido virtual agora" com pelo menos 25GB de Tamanho de disco. Não selecione nenhum campo adicional e clique em "Próximo".
.8 - Clique em "Finalizar".  
 
 Após completar a instalação, continue para o "Passo 2". 

 2 - Ubuntu 22.04 para seu Desktop ou Notebook. 

Nota: É importante que o usuário possua um pendrive com o bootavel do Ubuntu 22.04, caso não possua, siga o tutorial do link a seguir para realizar o processo de criação de USB inicializável através do Rufus. O tutorial abaixo também ensina a utilizar o VirtualBox passo a passo, caso não tenha interesse, pule para a etapa "Como Continuar com o Instalador do Ubuntu".  
Tutorial para instalação do Ubuntu: https://www.hostinger.com.br/tutoriais/como-instalar-ubuntu. 
No processo de instalação, lembrar de selecionar o layout de teclado Português(Brasil) e apagar o disco (Erase disk and install Ubuntu).

Passo 2: Configurando o Sistema.  SCRIPT EM SHELL 

1 - Com o Sistema Operacional inicializado, vá para a aba de aplicativos localizada no canto inferior esquerdo e clique em "Show Applications". 

2 - Clique em "Terminal" e aguarde ser executado. 

3 - Execute os seguintes comandos: 
    sudo apt update
    sudo apt upgrade 

4 - Adicionando compatibilidade 32bits:
    sudo dpkg --add-architecture i386
    sudo apt-get update
    sudo apt-get install libc6:i386 libncurses5:i386 libstdc++6:i386

5 - Criando uma chave para o GitHub: 
- Para criar uma chave *ed25519*, executar: 
    ssh-keygen -t ed25519 -C "your_email@example.com"
- Para criar uma chave rsa, executar: 
    ssh-keygen -t rsa -b 4096 -C "your_email@example.com"
Nota: anote a palavra usada para autenticação. 
- Rodar o ssh-agent: 
    eval $(ssh-agent -s)
- Incluir a chave privada: 
    ssh-add ~/.ssh/id_ed25519
- Copiar a chave
    cat ~/.ssh/id_ed25519.pub

6 - Adicionando chave SSH ao GitHub: 
- Acesse com seu usuário: github.com/settings/keys 
- Clique em "New SSH key", crie um Título para sua chave e cole o conteúdo obtido apos executar o comando "cat ~/.ssh/id_ed25519.pub"
Um tutorial mais detalhado pode ser encontrado em: https://docs.github.com/pt/authentication/connecting-to-github-with-ssh/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent

7 - Adicionando TOKEN ao 


7 - Realizando compatibilidade de HASH: 
- Execute o comando: 
    sudo nano /etc/ssh/ssh_config.d/my.conf
- No arquivo de texto aberto via NANO, cole as seguintes informações: 
    HostKeyAlgorithms ssh-rsa,ssh-dss
    PubkeyAcceptedKeyTypes ssh-rsa,ssh-dss
- Feche o arquivo com CTRL + X, selecionando "Y" (yes) para salvar as alterações. 

8 - Adicionando VPN ao computador ou ao VirtualBox: 
- Fazer o pedido da VPN e salvar em uma pasta do S.O
Acessar: Settings > network > VPN > Import from file... 
- Selecione o arquivo "seu_nome.ovpn"
    sudo apt install wget 
    sudo apt update
    sudo apt upgrade 

9 - Configurando o GitHub: 









_____________________________________________________________________
9 - Criar uma pasta Boardcomm dentro do seu computador
    mkdir Boardcomm 

10 - Copiando arquivo da pasta shared com jogo donuts: 
COPIANDO ARQUIVOS PARA RODAR JOGO COM PLACA: 

cd Boardcomm 
sudo scp -P 1337 -r [mariana@192.168.1.241](mailto:mariana@192.168.1.241):/home/shared/orion-dev/dev_game/adm.data .
sudo scp -P 1337 -r [mariana@192.168.1.241](mailto:mariana@192.168.1.241):/home/shared/orion-dev/dev_game/hli.pic30.bin .
sudo scp -P 1337 -r [mariana@192.168.1.241](mailto:mariana@192.168.1.241):/home/shared/orion-dev/dev_game/tool .

11 - COPIANDO JOGO DO SHARED:
sudo scp -P 1337 -r [mariana@192.168.1.241](mailto:mariana@192.168.1.241):/home/shared/orion-dev/games .

12 - UNZIP jogo 
sudo Boardcomm/games 
sudo unzip donuts-plus.zip
sudo rm donuts-plus.zip

13 - Baixando libs 

L1Ovhgrk
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/liblber-2.4.so.2.5.4 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libldap_r-2.4.so.2.5.4 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libldap_r-2.4.so.2.5.4 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libmysqlclient.so.16.0.0 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libsasl2.so.2.0.23 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libgnutls.so.26.14.12 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libtasn1.so.3.1.7 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libgcrypt.so.11.5.2 /lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libstdc++.so.6.0.13 /usr/lib/libstdc++.so.6.0.13
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libQtCore.so.4.6.2 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libQtGui.so.4.6.2 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libQtNetwork.so.4.6.2 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libSDL-1.2.so.0.11.3 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libz.so.1.2.3.3 /lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libgthread-2.0.so.0.2400.1 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libglib-2.0.so.0.2400.1 /lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libfontconfig.so.1.4.4 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libaudio.so.2.4 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libpng.so /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libfreetype.so.6.3.22 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libgobject-2.0.so.0.2400.1 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libSM.so.6.0.1 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libICE.so.6.3.0 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libXrender.so.1.3.0 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libXext.so.6.4.0 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libX11.so.6.3.0 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libpulse-simple.so.0.0.3 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libpulse.so.0.12.2 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libdirectfb-1.2.so.0.8.0 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libfusion-1.2.so.0.8.0 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libdirect-1.2.so.0.8.0 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libpcre.so.3.12.1 /lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libexpat.so.1.5.2 /lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libXt.so.6.0.0 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libXau.so.6.0.0 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libuuid.so.1.3.0 /lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libxcb.so.1.1.0 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libXtst.so.6.1.0 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libXdmcp.so.6.0.0 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libwrap.so.0.7.6 /lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libsndfile.so.1.0.21 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libdbus-1.so.3.4.0 /lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libXi.so.6.1.0 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libFLAC.so.8.2.0 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libvorbisenc.so.2.0.6 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libvorbis.so.0.4.3 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libogg.so.0.6.0 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libpulsecommon-0.9.21.so /usr/lib
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libaviplay-0.7.so.0.0.48 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libevent-1.4.so.2.1.3 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libcurl.so /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libhiredis.so.0.13 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libjson-c.so.3.0.0 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libmysqlpp.so.3.0.9 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libavcodec.so.52.20.1 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libavformat.so.52.31.0 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libXinerama.so.1.0.0 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libXxf86vm.so.1.0.0 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libXxf86dga.so.1.0.0 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libXft.so.2.1.13 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libmysqlclient.so.16.0.0 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libavutil.so.49.15.0 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libgsm.so.1.0.12 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libschroedinger-1.0.so.0.2.0 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libspeex.so.1.5.0 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libtheora.so.0.3.10 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libbz2.so.1.0.4 /lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/liboil-0.3.so.0.3.0 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/liblber-2.4.so.2.5.4 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libldap_r-2.4.so.2.5.4 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libssl.so.0.9.8 /lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libcrypto.so.0.9.8 /lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libsasl2.so.2.0.23 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libgssapi_krb5.so.2.2 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libgnutls.so.26.14.12 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libkrb5.so.3.3 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libk5crypto.so.3.1 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libcom_err.so.2.1 /lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libkrb5support.so.0.1 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libkeyutils-1.2.so /lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libtasn1.so.3.1.7 /usr/lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libgcrypt.so.11.5.2 /lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libgpg-error.so.0.4.0 /lib/
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libqrencode.so.3.9.0 /usr/local/lib/libqrencode.so.3.9.0
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libstdc++.so.6.0.13 /usr/lib
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libpthread-2.11.1.so /lib
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libm-2.11.1.so /lib
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/libgcc_s.so.1 /lib
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/lib/libc-2.11.1.so /lib
sudo scp -P 1337 mariana@192.168.1.241:/home/shared/orion-dev/libs/lib/ld-2.11.1.so /lib



sudo ln -s /usr/lib/liblber-2.4.so.2.5.4 /usr/lib/liblber-2.4.so.2
sudo ln -s /usr/lib/libldap_r-2.4.so.2.5.4 /usr/lib/libldap_r-2.4.so.2
sudo ln -s /usr/lib/libmysqlclient.so.16.0.0 /usr/lib/libmysqlclient.so.16
sudo ln -s /usr/lib/libsasl2.so.2.0.23 /usr/lib/libsasl2.so.2
sudo ln -s /usr/lib/libgnutls.so.26.14.12 /usr/lib/libgnutls.so.26
sudo ln -s /usr/lib/libtasn1.so.3.1.7 /usr/lib/libtasn1.so.3
sudo ln -s /lib/libgcrypt.so.11.5.2 /lib/libgcrypt.so.11
sudo ln -s /home/shared/orion-dev/libs/libstdc++.so.6.0.13 /usr/lib/libstdc++.so.6
sudo ln -s /usr/lib/libQtCore.so.4.6.2 /usr/lib/libQtCore.so.4
sudo ln -s /usr/lib/libQtGui.so.4.6.2 /usr/lib/libQtGui.so.4
sudo ln -s /usr/lib/libQtNetwork.so.4.6.2 /usr/lib/libQtNetwork.so.4
sudo ln -s /usr/lib/libSDL-1.2.so.0.11.3 /usr/lib/libSDL-1.2.so.0
sudo ln -s /lib/libz.so.1.2.3.3 /lib/libz.so.1
sudo ln -s /usr/lib/libgthread-2.0.so.0.2400.1 /usr/lib/libgthread-2.0.so.0
sudo ln -s /lib/libglib-2.0.so.0.2400.1 /lib/libglib-2.0.so.0
sudo ln -s /usr/lib/libfontconfig.so.1.4.4 /usr/lib/libfontconfig.so.1
sudo ln -s /usr/lib/libaudio.so.2.4 /usr/lib/libaudio.so.2
sudo ln -s /usr/lib/libpng.so /usr/lib/libpng12.so.0
sudo ln -s /usr/lib/libfreetype.so.6.3.22 /usr/lib/libfreetype.so.6
sudo ln -s /usr/lib/libgobject-2.0.so.0.2400.1 /usr/lib/libgobject-2.0.so.0
sudo ln -s /usr/lib/libSM.so.6.0.1 /usr/lib/libSM.so.6
sudo ln -s /usr/lib/libICE.so.6.3.0 /usr/lib/libICE.so.6
sudo ln -s /usr/lib/libXrender.so.1.3.0 /usr/lib/libXrender.so.1
sudo ln -s /usr/lib/libXext.so.6.4.0 /usr/lib/libXext.so.6
sudo ln -s /usr/lib/libX11.so.6.3.0 /usr/lib/libX11.so.6
sudo ln -s /usr/lib/libpulse-simple.so.0.0.3 /usr/lib/libpulse-simple.so.0
sudo ln -s /usr/lib/libpulse.so.0.12.2 /usr/lib/libpulse.so.0
sudo ln -s /usr/lib/libdirectfb-1.2.so.0.8.0 /usr/lib/libdirectfb-1.2.so.0
sudo ln -s /usr/lib/libfusion-1.2.so.0.8.0 /usr/lib/libfusion-1.2.so.0
sudo ln -s /usr/lib/libdirect-1.2.so.0.8.0 /usr/lib/libdirect-1.2.so.0
sudo ln -s /lib/libpcre.so.3.12.1 /lib/libpcre.so.3
sudo ln -s /lib/libexpat.so.1.5.2 /lib/libexpat.so.1
sudo ln -s /usr/lib/libXt.so.6.0.0 /usr/lib/libXt.so.6
sudo ln -s /usr/lib/libXau.so.6.0.0 /usr/lib/libXau.so.6
sudo ln -s /lib/libuuid.so.1.3.0 /lib/libuuid.so.1
sudo ln -s /usr/lib/libxcb.so.1.1.0 /usr/lib/libxcb.so.1
sudo ln -s /usr/lib/libXtst.so.6.1.0 /usr/lib/libXtst.so.6
sudo ln -s /usr/lib/libXdmcp.so.6.0.0 /usr/lib/libXdmcp.so.6
sudo ln -s /lib/libwrap.so.0.7.6 /lib/libwrap.so.0
sudo ln -s /usr/lib/libsndfile.so.1.0.21 /usr/lib/libsndfile.so.1
sudo ln -s /lib/libdbus-1.so.3.4.0 /lib/libdbus-1.so.3
sudo ln -s /usr/lib/libXi.so.6.1.0 /usr/lib/libXi.so.6
sudo ln -s /usr/lib/libFLAC.so.8.2.0 /usr/lib/libFLAC.so.8
sudo ln -s /usr/lib/libvorbisenc.so.2.0.6 /usr/lib/libvorbisenc.so.2
sudo ln -s /usr/lib/libvorbis.so.0.4.3 /usr/lib/libvorbis.so.0
sudo ln -s /usr/lib/libogg.so.0.6.0 /usr/lib/libogg.so.0
sudo ln -s /usr/lib/libaviplay-0.7.so.0.0.48 /usr/lib/libaviplay-0.7.so.0
sudo ln -s /usr/lib/libevent-1.4.so.2.1.3 /usr/lib/libevent-1.4.so.2
sudo ln -s /usr/local/lib/libcurl.so /usr/lib/libcurl.so
sudo ln -s /usr/local/lib/libhiredis.so.0.13 /usr/lib/libhiredis.so.0.13
sudo ln -s /usr/lib/libjson-c.so.3.0.0 /usr/lib/libjson-c.so.3
sudo ln -s /usr/lib/libmysqlpp.so.3.0.9 /usr/lib/libmysqlpp.so.3
sudo ln -s /usr/lib/libavcodec.so.52.20.1 /usr/lib/libavcodec.so.52
sudo ln -s /usr/lib/libavformat.so.52.31.0 /usr/lib/libavformat.so.52
sudo ln -s /usr/lib/libXinerama.so.1.0.0 /usr/lib/libXinerama.so.1
sudo ln -s /usr/lib/libXxf86vm.so.1.0.0 /usr/lib/libXxf86vm.so.1
sudo ln -s /usr/lib/libXxf86dga.so.1.0.0 /usr/lib/libXxf86dga.so.1
sudo ln -s /usr/lib/libXft.so.2.1.13 /usr/lib/libXft.so.2
sudo ln -s /usr/lib/libmysqlclient.so.16.0.0 /usr/lib/libmysqlclient.so.16
sudo ln -s /usr/lib/libavutil.so.49.15.0 /usr/lib/libavutil.so.49
sudo ln -s /usr/lib/libgsm.so.1.0.12 /usr/lib/libgsm.so.1
sudo ln -s /usr/lib/libschroedinger-1.0.so.0.2.0 /usr/lib/libschroedinger-1.0.so.0
sudo ln -s /usr/lib/libspeex.so.1.5.0 /usr/lib/libspeex.so.1
sudo ln -s /usr/lib/libtheora.so.0.3.10 /usr/lib/libtheora.so.0
sudo ln -s /lib/libbz2.so.1.0.4 /lib/libbz2.so.1.0
sudo ln -s /usr/lib/liboil-0.3.so.0.3.0 /usr/lib/liboil-0.3.so.0
sudo ln -s /usr/lib/liblber-2.4.so.2.5.4 /usr/lib/liblber-2.4.so.2
sudo ln -s /usr/lib/libldap_r-2.4.so.2.5.4 /usr/lib/libldap_r-2.4.so.2
sudo ln -s /usr/lib/libsasl2.so.2.0.23 /usr/lib/libsasl2.so.2
sudo ln -s /usr/lib/libgssapi_krb5.so.2.2 /usr/lib/libgssapi_krb5.so.2
sudo ln -s /usr/lib/libgnutls.so.26.14.12 /usr/lib/libgnutls.so.26
sudo ln -s /usr/lib/libkrb5.so.3.3 /usr/lib/libkrb5.so.3
sudo ln -s /usr/lib/libk5crypto.so.3.1 /usr/lib/libk5crypto.so.3
sudo ln -s /lib/libcom_err.so.2.1 /lib/libcom_err.so.2
sudo ln -s /usr/lib/libkrb5support.so.0.1 /usr/lib/libkrb5support.so.0
sudo ln -s /lib/libkeyutils-1.2.so /lib/libkeyutils.so.1
sudo ln -s /usr/lib/libtasn1.so.3.1.7 /usr/lib/libtasn1.so.3
sudo ln -s /lib/libgcrypt.so.11.5.2 /lib/libgcrypt.so.11
sudo ln -s /lib/libgpg-error.so.0.4.0 /lib/libgpg-error.so.0
sudo ln -s /usr/local/lib/libqrencode.so.3.9.0 /usr/lib/libqrencode.so.3
sudo ln -s /lib/libpthread-2.11.1.so /lib/libpthread.so.0
sudo ln -s /lib/libm-2.11.1.so /lib/libm.so.6
sudo ln -s /lib/libc-2.11.1.so /lib/libc.so.6
sudo ln -s /lib/ld-2.11.1.so /lib/ld-linux.so.2
