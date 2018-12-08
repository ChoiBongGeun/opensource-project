#include <stdio.h>
#include <string.h>

void Menu();
void FileEncoding();
void FileDecoding();

void main()
{
        Menu();
}
void Menu()
{
    int choice;
    for(;;)
    {
        printf("1.암호화\n 2.복호화\n 5종료\n");
        printf("입력 : ");
        scanf("%d",&choice);

        if(choice <3)
        {
            break;
        }
        else
        {
            continue;
        }
    }
    switch(choice)
    {
        case 1 :
            FileEncoding();
            break;
        case 2 : 
            FileDecoding();
            break;
        case 3 :
            exit(1);
    }
}

void FileEncoding()
{
    FILE * FileCreate = NULL;
    int File;
    char Type[27];

    system("clear");
    printf("확장자도 입력해줘야 합니다\n");
    getchar();

    for(;;)
    {
        printf("암호화 할 파일을 입력 해주세요:");
        gets(Type);

        FileCreate=fopen(Type,"rb");

        if(FileCreate==NULL)
        {
            printf("\n 파일 없다 \n");
            continue;
        }
        else
        {
            FILE * CreateFile;
            char NameType[27];

            printf("\n 생성될 파일명을 적어주세요 :");
            gets(NameType);

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
    }
    fclose(FileCreate);
}
void FileDecoding()
{
    FILE * DeCodFileCreate = NULL;
    int File;
    char DeType[27];

    
    printf("확장자도 입력해줘야 합니다\n");
    getchar();

    for(;;)
    {
        printf("복호화 할 파일을 입력 해주세요:");
        gets(DeType);

        DeCodFileCreate=fopen(DeType,"rb");

        if(DeCodFileCreate==NULL)
        {
            printf("\n 파일 없다 \n");
            continue;
        }
        else
        {
            FILE * DeCodCreateFile;
            char DeCodNameType[27];

            printf("\n 생성될 파일명을 적어주세요 :");
            gets(DeCodNameType);

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
    }
    fclose(DeCodFileCreate);
}