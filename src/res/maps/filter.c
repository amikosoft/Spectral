// extract zxdb id + map size (bytes) + author

struct maps_t {
    const char *line;
    const char *id;
    const char *bytes;
    const char *author;
} maps[4096] = {0};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    void *data = 0;
    for( FILE *fp = fopen("mapy3.txt", "rb"); fp; fclose(fp), fp = 0) {
        fseek(fp, 0L, SEEK_END);
        size_t sz = ftell(fp);
        fseek(fp, 0L, SEEK_SET);
        data = calloc(1, sz);
        fread(data, 1, sz, fp);

        char buf[4096];
        fseek(fp, 0L, SEEK_SET);
        while( fgets(buf, 4096, fp) ) {

            char *tabs[16] = {0}; tabs[0] = strstr(data, buf);
            for( int i = 0; i < 16 && tabs[i] && strchr(tabs[i], ';'); ++i )
                tabs[i+1] = strchr(tabs[i], ';') + 1;

            if( atoi(tabs[12]) == 0 ) continue; // exclude remakes and non-zxdb entries

            static int c = 0;
            maps[c].line = tabs[0];
            maps[c].id = tabs[12];
            maps[c].bytes = tabs[3];
            maps[c].author = tabs[11]; // maps[c].author[strcspn(maps[c].author, ";")] = '\0';
            ++c;
        }
    }

    for( int i = 0; maps[i].id; ++i ) {
        printf("\t{%5d, %d, \"%.*s\"}, // %.*s\n", atoi(maps[i].id), atoi(maps[i].bytes), (int)strcspn(maps[i].author, ";"), maps[i].author, (int)strcspn(maps[i].line, "\n"), maps[i].line);
    }
}
