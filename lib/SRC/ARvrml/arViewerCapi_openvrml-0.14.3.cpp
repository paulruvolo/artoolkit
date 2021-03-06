#include <AR/arvrml.h>
#include "arViewer_openvrml-0.14.3.h"
#include <iostream>
#include <vector>
#include <string>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif
#include <stdio.h>
#include <string.h>

#define  AR_VRML_MAX   100

static arVrmlViewer    *viewer[AR_VRML_MAX];
static int              init = 1;
static int              vrID = -1;

static char *get_buff( char *buf, int n, FILE *fp );


int arVrmlLoadFile(const char *file)
{
  
    FILE             *fp;
    openvrml::browser * myBrowser = 0;
    char             buf[256], buf1[256];
    char             buf2[256];
    int              id;
    int              i;

    if( init ) {
        for( i = 0; i < AR_VRML_MAX; i++ ) viewer[i] = NULL;
        init = 0;
    }
    for( i = 0; i < AR_VRML_MAX; i++ ) {
        if( viewer[i] == NULL ) break;
    }
    if( i == AR_VRML_MAX ) return -1;
    id = i;


    if( (fp=fopen(file, "r")) == NULL ) return -1;

    get_buff(buf, 256, fp);
    if( sscanf(buf, "%s", buf1) != 1 ) {fclose(fp); return -1;}
    for( i = 0; file[i] != '\0'; i++ ) buf2[i] = file[i];
    for( ; i >= 0; i-- ) {
        if( buf2[i] == '/' ) break;
    }
    buf2[i+1] = '\0';
    sprintf(buf, "%s%s", buf2, buf1);

    myBrowser = new openvrml::browser(std::cout, std::cerr);
    if( !myBrowser) {fclose(fp); return -1;}

    std::vector<std::string> uri(1, buf);
    std::vector<std::string> parameter; 
    myBrowser->load_url(uri, parameter);

    viewer[id] = new arVrmlViewer(*myBrowser);
    if(!viewer[id]) 
    {
        delete myBrowser;
        fclose(fp);
        return -1;
    }
    strcpy( viewer[id]->filename, buf );
    
    get_buff(buf, 256, fp);
    if( sscanf(buf, "%lf %lf %lf", &viewer[id]->translation[0], 
        &viewer[id]->translation[1], &viewer[id]->translation[2]) != 3 ) {
        delete viewer[id];
        viewer[id] = NULL;
        fclose(fp);
        return -1;
    }

    get_buff(buf, 256, fp);
    if( sscanf(buf, "%lf %lf %lf %lf", &viewer[id]->rotation[0],
        &viewer[id]->rotation[1], &viewer[id]->rotation[2], &viewer[id]->rotation[3]) != 4 ) {
        delete viewer[id];
        viewer[id] = NULL;
        fclose(fp);
        return -1;
    }

    get_buff(buf, 256, fp);
    if( sscanf(buf, "%lf %lf %lf", &viewer[id]->scale[0], &viewer[id]->scale[1],
               &viewer[id]->scale[2]) != 3 ) {
        delete viewer[id];
        viewer[id] = NULL;
        fclose(fp);
        return -1;
    }
    fclose(fp);

    return id;
}

int arVrmlFree( int id )
{
    if( viewer[id] == NULL ) return -1;

    delete viewer[id];
    viewer[id] = NULL;

    if( vrID == id ) {
        vrID = -1;
    }

    return 0;
}

int arVrmlTimerUpdate()
{
     int     i;

    for( i = 0; i < AR_VRML_MAX; i++ ) {
        if( viewer[i] == NULL ) continue;
        viewer[i]->timerUpdate();
    }
    return 0;
}

int arVrmlDraw( int id )
{
     if( viewer[id] == NULL ) return -1;
     viewer[id]->redraw();
     return 0;
}

int arVrmlSetInternalLight( int flag )
{
   int     i;

    if( flag ) {
        for( i = 0; i < AR_VRML_MAX; i++ ) {
            if( viewer[i] == NULL ) continue;
            viewer[i]->setInternalLight(true);
        }
    }
    else {
        for( i = 0; i < AR_VRML_MAX; i++ ) {
            if( viewer[i] == NULL ) continue;
            viewer[i]->setInternalLight(false);
        }
    }
  
    return 0;
}

static char *get_buff( char *buf, int n, FILE *fp )
{
    char *ret, buf1[256];

    for(;;) {
        ret = fgets( buf, n, fp );
        if( ret == NULL ) return(NULL);
        if( sscanf(buf, "%s", buf1) != 1 ) continue;

        if( buf1[0] != '#' ) return(ret);
    }
}
