/* zlib 의 사용하는 예*/
/* 컴파일: gcc -Wall comptest.c -o comptest -lz */
/* -lz 는 libz.a 을 링크한다 (아마 /usr(/local)/lib 에 있다）*/

#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include <zlib.h>               /* /usr(/local)/include/zlib.h */

#define INBUFSIZ   1024         /* 입력 버퍼 사이즈（임의） */
#define OUTBUFSIZ  1024         /* 출력 버퍼 사이즈（임의） */

z_stream z;                     /* 라이브러리와 주고받기 위한 구조체 */

char inbuf[INBUFSIZ];           /* 입력 버퍼 */
char outbuf[OUTBUFSIZ];         /* 출력 버퍼 */
FILE *fin, *fout;               /* 입력.출력 파일 */

void do_compress(void)          /* 압축 */
{
    int count, flush, status;

    /* 모든 메모리 관리를 라이브러리에 맡긴다 */
    z.zalloc = Z_NULL;
    z.zfree = Z_NULL;
    z.opaque = Z_NULL;

    /* 초기화 */
    /* 제2인수는 압축의 정도. 0～9 의 범위의 정수로，0 은 무압축 */
    /* Z_DEFAULT_COMPRESSION (= 6) 이 표준 */
    if (deflateInit(&z, Z_DEFAULT_COMPRESSION) != Z_OK) {
        fprintf(stderr, "deflateInit: %s\n", (z.msg) ? z.msg : "???");
        exit(1);
    }

    z.avail_in = 0;             /* 입력버퍼 안의 데이터 바이트 수 */
    z.next_out = outbuf;        /* 출력 포인터 */
    z.avail_out = OUTBUFSIZ;    /* 출력 버퍼의 사이즈 */

    /* 통상은 deflate() 의 제2인수는 Z_NO_FLUSH 로 해서 호출한다 */
    flush = Z_NO_FLUSH;

    while (1) {
        if (z.avail_in == 0) {  /* 입력이 다 되면 */
            z.next_in = inbuf;  /* 입력 포인터를 입력 버퍼의 선두에 */
            z.avail_in = fread(inbuf, 1, INBUFSIZ, fin); /* 데이터를 읽어들임 */

            /* 입력이 마지막이 되면 deflate() 의 제2인수는 Z_FINISH 로 한다 */
            if (z.avail_in < INBUFSIZ) flush = Z_FINISH;
        }
        status = deflate(&z, flush); /* 압축한다 */
        if (status == Z_STREAM_END) break; /* 완료 */
        if (status != Z_OK) {   /* 에러 */
            fprintf(stderr, "deflate: %s\n", (z.msg) ? z.msg : "???");
            exit(1);
        }
        if (z.avail_out == 0) { /* 출력버퍼가 다 되면 */
            /* 정리해서 써 낸다 */
            if (fwrite(outbuf, 1, OUTBUFSIZ, fout) != OUTBUFSIZ) {
                fprintf(stderr, "Write error\n");
                exit(1);
            }
            z.next_out = outbuf; /* 출력버퍼 잔량을 원래로 되돌린다 */
            z.avail_out = OUTBUFSIZ; /* 출력 포인터를 원래로 되돌림 */
        }
    }

    /* 나머지를 뱉어낸다 */
    if ((count = OUTBUFSIZ - z.avail_out) != 0) {
        if (fwrite(outbuf, 1, count, fout) != count) {
            fprintf(stderr, "Write error\n");
            exit(1);
        }
    }

    /* 후처리 */
    if (deflateEnd(&z) != Z_OK) {
        fprintf(stderr, "deflateEnd: %s\n", (z.msg) ? z.msg : "???");
        exit(1);
    }
}

void do_decompress(void)        /* 전개（복원） */
{
    int count, status;

    /* 모든 메모리 관리를 라이브러리에 맡긴다 */
    z.zalloc = Z_NULL;
    z.zfree = Z_NULL;
    z.opaque = Z_NULL;

    /* 초기화 */
    z.next_in = Z_NULL;
    z.avail_in = 0;
    if (inflateInit(&z) != Z_OK) {
        fprintf(stderr, "inflateInit: %s\n", (z.msg) ? z.msg : "???");
        exit(1);
    }

    z.next_out = outbuf;        /* 출력 포인터 */
    z.avail_out = OUTBUFSIZ;    /* 출력 버퍼 잔량 */
    status = Z_OK;

    while (status != Z_STREAM_END) {
        if (z.avail_in == 0) {  /* 입력 잔량이 제로가 되면 */
            z.next_in = inbuf;  /* 입력 포인터를 원래로 되돌림 */
            z.avail_in = fread(inbuf, 1, INBUFSIZ, fin); /* 데이터를 읽음 */
        }
        status = inflate(&z, Z_NO_FLUSH); /* 전개 */
        if (status == Z_STREAM_END) break; /* 완료 */
        if (status != Z_OK) {   /* 에러 */
            fprintf(stderr, "inflate: %s\n", (z.msg) ? z.msg : "???");
            exit(1);
        }
        if (z.avail_out == 0) { /* 출력버퍼가 다 되면 */
            /* 정리해서 써 낸다 */
            if (fwrite(outbuf, 1, OUTBUFSIZ, fout) != OUTBUFSIZ) {
                fprintf(stderr, "Write error\n");
                exit(1);
            }
            z.next_out = outbuf; /* 출력 포인터를 원래로 되돌림 */
            z.avail_out = OUTBUFSIZ; /* 출력 버퍼 잔량을 원래로 되돌림 */
        }
    }

    /* 나머지를 뱉어낸다 */
    if ((count = OUTBUFSIZ - z.avail_out) != 0) {
        if (fwrite(outbuf, 1, count, fout) != count) {
            fprintf(stderr, "Write error\n");
            exit(1);
        }
    }

    /* 후처리 */
    if (inflateEnd(&z) != Z_OK) {
        fprintf(stderr, "inflateEnd: %s\n", (z.msg) ? z.msg : "???");
        exit(1);
    }
}

void FileEncoding(char *Type)
{
    FILE * FileCreate = NULL;
    int File;


    for(;;)
    {
        FileCreate=fopen(Type,"rb");

        
            FILE * CreateFile;
            char NameType[27];
            char s[]=".lock";
            
            strcat(NameType, Type);
            strcat(NameType, s);
            CreateFile=fopen(NameType,"wb");

            while ((File = fgetc(FileCreate)) != EOF )
            {
                File^=13;
                printf(",");
                fputc(File, CreateFile);
            }
            fclose(CreateFile);
                printf("\n종료합니다\n");
                break;
        
    }
    fclose(FileCreate);
}

void FileDecoding(char *DeType,char *DeCodNameType)
{
    FILE * DeCodFileCreate = NULL;
    int File;
  

    for(;;)
    {

        DeCodFileCreate=fopen(DeType,"rb");

            FILE * DeCodCreateFile;
           

             DeCodCreateFile=fopen(DeCodNameType,"wb");

            while ((File = fgetc(DeCodFileCreate)) != EOF )
            {
                File^=13;
                printf(",");
                fputc(File, DeCodCreateFile);
            }
            fclose(DeCodCreateFile);
                printf("\n종료합니다\n");
                break;
        
    }
    fclose(DeCodFileCreate);
}
int main(int argc, char *argv[])
{
    int c;
    int ck;
    int du;
    int d;
 

    if (argc != 4) {
        fprintf(stderr, "Usage: comptest flag infile outfile\n");
        fprintf(stderr, "  flag: c=compress d=decompress\n");
        exit(0);
    }
    if (argv[1][0] == 'c') {
        c = 1;
    } else if (argv[1][0] == 'd') {
        d = 1;
    } else if (argv[1][0] == 'a') {
        ck = 1;
    } else if (argv[1][0] == 'b') {
        du = 1;
    }else {
        fprintf(stderr, "Unknown flag: %s\n", argv[1]);
        exit(1);
    }
    if ((fin = fopen(argv[2], "r")) == NULL) {
        fprintf(stderr, "Can't open %s\n", argv[2]);
        exit(1);
    }
    if ((fout = fopen(argv[3], "w")) == NULL) {
        fprintf(stderr, "Can't open %s\n", argv[3]);
        exit(1);
    }
    if (c == 1) {
        do_compress(); 
    }
    else if(ck==1){
        pid_t pid;
        pid = fork();
        if (pid ==0){
         do_compress();   
        }
        else{
            waitpid(pid,NULL,0);
            FileEncoding(argv[3]);
            remove(argv[3]);
        } 
    }
    else if(du == 1){
          FileDecoding(argv[2],argv[3]);  
    }
    else if (d) do_decompress();
    fclose(fin);
    fclose(fout);
    return 0;
}
