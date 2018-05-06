
Manual do Script converter.py
*****************************

O script converter.py converte um modelo no formato wavefront para dados acessíveis em C++. Os dados são convertidos para vetores que podem ser facilmente manipulados usando OpenGL. Suponha que você tenha um arquivo cubo.obj no mesmo direório do script converter.py. A sintaxe gerla de uso do script converter.py é:

Para converter o arquivo cubo.obj para C++, utilize o seguinte comando:

$> python converter.py <nome_do_arquivo_obj> <prefixo_das_variaveis_de_dados>

onde <nome_do_arquivo_obj> é um nome do arquivo no formato wavefront (sem extensão) que contenha um modelo 3D; já <prefixo_das_variaveis_de_dados> é um prefixo que será adicionado em cada tipo de dado do objeto 3d exportado. Os tipos de dados do objeto 3d são faces, vértices, normais e índices. Um modelo 3D como malha de polígonos tem vértices e faces; as faces indicam como combinar os vértices para formarem os polígonos da malha de polígonos do objeto 3d. Por exemplo, Para converter o arquivo cubo.obj para C++, utilize o seguinte comando:

$> python converter.py cubo cubo

Este comando gerará uma variável de dados para cada informação 3D relevante do cubo. Os vértices ficam em um vetor nomeado cubo_vertices; as normais, em um vetor cubo_normals. Já as faces e seus índices de vértices ficam em um vetor apontado pela variável cubo_faceindex; finalmente, o índices de normais das faces são gerados em um vetor cubo_normalindex. Além disso, o script export contagens como a quantidade de vértices (variável cubo_numberOfVertices), a quantidade de faces (a variável cubo_numberOfFaces) e a quantidade de índices por face (variável cubo_verticesInFace). Portanto, com o comando acima, o arquivo gerado será cubo.cpp a partir do modelo cubo.obj e as variáveis geradas serão: cubo_vertices, cubo_normais, cubo_faceindex, cubo_normalindex. Se quisermos gerar as variáveis com outro prefixo, por exemplo, obj3d, podemos executar o comando do seguinte modo:

$> python converter.py cubo obj3d

OBSERVAÇÃO: o script converter.py foi desenvolvido em Python 3.

----------------------------------------------------------------
Quaisquer dúvidas, envie um email para gilzamir_gomes@uvanet.br