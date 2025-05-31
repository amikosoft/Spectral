cl utf8.c
curl -LO https://maps.speccy.cz/mapy2.txt
utf8 < mapy2.txt > mapy3.txt

echo struct map_t { unsigned id, bytes; const char *author; } maps[] = { > maps.h
cl filter.c %* && (filter | sort) >> maps.h
echo }; >> maps.h

echo #include "maps.h" > maps.c
echo main(){} >> maps.c
cl maps.c || (echo Error! && exit /b)

del *.exe
del *.obj
echo Ok
