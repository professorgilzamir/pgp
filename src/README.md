*******************
COMPILANDO NO LINUX
*******************

Entre no diretório do exemplo que você quer compilar e execute o seguinte comando

$> make

Para executar o programa gerado, execute o programa do seguinte modo:

$> ./<nome_do_programa>

onde <nome_do_programa> é o nome do executável gerado na compilação. O nome do executável gerado é a parte do nome do diretório após a underline. Por exemplo, se estiver compilando o exemplo 2 do capítulo 2, o diretório do exemplo tem o seguinte nome cap2_ex2, assim, o nome do executável será ex2.


*********************
COMPILANDO NO MACOSX
*********************

O procedimento para compilar os exemplos no MacOSX é parecido com o procedimento para se fazer isso no Linux. Contudo, ao executar o comando make, deve-se especificar a plataforma alvo, que é o Linux padrão. No geral, o comando para compilar é:

$> make PLAT=<PLATAFORMA>

onde <PLATAFORMA> pode ser macosx ou linux. Portanto, para MacOSX, o comando para compilação é:

$> make PLAT=macosx

A nomeação do executável gerado segue as mesmas regras do caso do Linux.