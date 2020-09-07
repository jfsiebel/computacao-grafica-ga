Run project from command line:

```bash
g++ -framework Cocoa -framework OpenGL -framework IOKit -
o arq main.c -I include -I /sw/include -I /usr/local/
include -I include libGLEW.a libglfw3.a
```