// THIS FILE IS AUTOMATICALLY GENERATED
// Please do not modify directly.
// For information on how to regenerate or extend
// see: /Code/Tools/glad/README

#include <stdio.h>
#include <string.h>
#include <DinrusX/DinrusXRend/glad_egl.h>

int gladLoadEGL(void) {
    return gladLoadEGLLoader((GLADloadproc)eglGetProcAddress);
}

static void find_extensionsEGL(void) {
}

static void find_coreEGL(void) {
}

int gladLoadEGLLoader(GLADloadproc load) {
	find_coreEGL();

	find_extensionsEGL();
	return 1;
}
