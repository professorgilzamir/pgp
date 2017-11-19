/* Usando o padrão de saída fprintf */
#include <stdio.h>
#include <stdlib.h>
/* Usando o GLUT com gerenciador de janelas */
#include <OpenGL/gl3.h>
#define __gl_h_
#include <GLUT/glut.h>

/* COLOCAREMOS AS VARIAVEIS GLOBAIS AQUI MAIS TARDE */

int inicializar(void)
{
  /* IMPLEMENTE AQUI */
  return 1;
}

void atualizarDesenho()
{
  /* IMPLEMENTE AQUI */
}

void finalizar()
{
  /* IMPLEMENTE AQUI */
}

int main(int argc, char* argv[])
{
  /* Funções necessárias para iniciar a GLUT */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(640, 480);
  glutCreateWindow("Meu Primeiro Triângulo");


  /* Quando as funções de inicialização são executadas sem erros,
  o programa pode iniciar os recursos */
  if (1 == inicializar())
  {
    /* Pode então mostrar se tudo correr bem */
    glutDisplayFunc(atualizarDesenho);
    glutMainLoop();
  }

  /* Se o ocasionalmente programa sair ,
  liberamos os recursos da memória e completaremos ele com sucesso*/
  finalizar();
  return EXIT_SUCCESS;
}
