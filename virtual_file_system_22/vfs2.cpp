#define _CRT_SECURE_NO_WARNINGS		//windows only
#define MAXINODE 50
#define WRITE 2
#define READ 1
#define MAXFILESIZE 1024
#define REGULAR 1
#define SPECIAL 2
#define START 0
#define END 2
#define CURRENT 1


#include<stdio.h>
#include<stdlib.h>
#include<io.h>		//windows only
#include<string.h>
#include<fcntl.h>
#include<dirent.h>


DIR *dirHandle=NULL;
dirent *fileHandle=NULL;
char path[]="C:\\Users\\LENOVO\\Desktop\\vfs\\";

typedef struct superblock
{
	int Totalinode;
	int Freeinode;
}SUPERBLOCK,*PSUPERBLOCK;



typedef struct inode
{
	char FileName[50];
	int FileSize;
	int FileActualSize;
	int FileType;
	char *Buffer;
	int LinkCount;
	int ReferenceCount;
	int Permission;
	int InodeNumber;
	struct inode *next;
}INODE,*PINODE,**PPINODE;



typedef struct filetable
{
	int readoffset;
	int writeoffset;
	int count;
	int mode;
	PINODE ptrinode;
}FILETABLE,*PFILETABLE;

typedef struct ufdt
{
	PFILETABLE ptrfiletable;
}UFDT;


UFDT UFDTArr[50];
SUPERBLOCK SUPERBLOCKobj;
PINODE head=NULL;


void DisplayHelp()
{
	printf("ls:List of all files\n");
	printf("clear:Clear console\n");
	printf("open:Open the specified file\n");
	printf("close:Close the specified file\n");
	printf("closeall:Close all opened files\n");
	printf("read:Read contents from file\n");
	printf("write:Write contents into file\n");
	printf("exit:Terminate the file system\n");
	printf("stat:Display information of file using name\n");
	printf("fstat:Display information of file using descriptor\n");
	printf("truncate:Remove all data from file\n");
	printf("rm:Delete the specified file\n");
}


void InitializeSuperBlock()
{
	int i=0;
	while(i<MAXINODE)
	{
		UFDTArr[i].ptrfiletable=NULL;
		i++;
	}
	SUPERBLOCKobj.Totalinode=MAXINODE;
	SUPERBLOCKobj.Freeinode=MAXINODE;
}


void CreateDILB()
{
	PINODE newn,temp=NULL;
	int i=1;
	while(i<=MAXINODE)
	{
		newn=(PINODE)malloc(sizeof(INODE));
		newn->LinkCount=newn->ReferenceCount=0;
		newn->FileSize=newn->FileType=0;
		newn->Buffer=NULL;
		newn->next=NULL;
		newn->InodeNumber=i;
		if(temp==NULL)
		{
			head=newn;
			temp=head;
		}
		else
		{
			temp->next=newn;
			temp=temp->next;
		}
		i++;
	}
}


void ls_file()
{
	printf("\nFile Name\tInode Number\tFile Size\tLink Count\n");
	printf("----------------------------------------------------------\n");
	PINODE temp=head;
	if(SUPERBLOCKobj.Freeinode==MAXINODE)
	{
		printf(".\t\t-\t\t-\t\t-\n");
		printf("..\t\t-\t\t-\t\t-\n");
		printf("There are no files\n");
		return;
	}
	
		printf(".\t\t-\t\t-\t\t-\n");
		printf("..\t\t-\t\t-\t\t-\n");
		while(temp->next!=NULL)
	{
		if(temp->FileType!=0)
		{
			printf("%s\t\t%d\t\t%d\t\t%d\n",temp->FileName,temp->InodeNumber,temp->FileSize,temp->LinkCount);
		}
		temp=temp->next;
	}
	printf("----------------------------------------------------------\n");
}




void man(char *name)
{
	if(name==NULL)
		return;

	if(stricmp(name,"create")==0)
	{
		printf("Description:Used to create a regular file\n");
		printf("Usage:create File_name Permission\n");
	}
	else if(stricmp(name,"read")==0)
	{
		printf("Description:Used to read data from regular files\n");
		printf("Usage:read File_name No_Of_Bytes_to_Read\n");
	}
	else if(stricmp(name,"write")==0)
	{
		printf("Description:Used to write into regular file\n");
		printf("Usage:write File_name\n");
	}
	else if(stricmp(name,"ls")==0)
	{
		printf("Description:Used to list all the information of files\n");
		printf("Usage:ls\n");
	}
	else if(stricmp(name,"stat")==0)
	{
		printf("Description:Used to display information of file\n");
		printf("Usage:stat File_name\n");
	}
	else if(stricmp(name,"fstat")==0)
	{
		printf("Description:Used to display information of file\n");
		printf("Usage:fstat File_name\n");
	}
	else if(stricmp(name,"truncate")==0)
	{
		printf("Description:Used to open an existing file\n");
		printf("Usage:truncate File_name\n");
	}
	else if(stricmp(name,"open")==0)
	{
		printf("Description:Used to open existing file\n");
		printf("Usage:open File_name mode\n");
	}
	else if(stricmp(name,"close")==0)
	{
		printf("Description:Used to close open file\n");
		printf("Usage:close File_name\n");
	}
	else if(stricmp(name,"closeall")==0)
	{
		printf("Description:Used to close all opened file\n");
		printf("Usage:closeall\n");
	}
	else if(stricmp(name,"lseek")==0)
	{
		printf("Description:Used to change file offset\n");
		printf("Usage:lseek File_name Change in Offset Startpoint\n");
	}
	else if(stricmp(name,"rm")==0)
	{
		printf("Description:Used to delete the file\n");
		printf("Usage:rm File_name\n");
	}
	else
	{
		printf("ERROR:No manual entry available...\n");
	}
}


PINODE Get_Inode(char *name)
{
	PINODE temp=head;

	if(name==NULL)
		return NULL;

	while(temp!=NULL)
	{
		if((stricmp(name,temp->FileName)==0)&&(temp->FileType!=0))
			break;
		temp=temp->next;
	}
	return temp;
}



int fstat_file(int fd)
{
	PINODE temp=head;
	if(fd=0)
		return -1;
	if(UFDTArr[fd].ptrfiletable==NULL)
		return -2;
	temp=UFDTArr[fd].ptrfiletable->ptrinode;
	printf("----------Statistical Information About File----------\n");
	printf("File Name:%s\n",temp->FileName);
	printf("Inode Number:%d\n",temp->InodeNumber);
	printf("File Size:%d\n",temp->FileSize);
	printf("Actual File Size:%d\n",temp->FileActualSize);
	printf("Link Count:%d\n",temp->LinkCount);
	printf("Reference Count:\n",temp->ReferenceCount);
	if(temp->Permission==1)
		printf("File Permission:Read Only...\n");
	else if(temp->Permission==2)
		printf("File Permission:Write Only...\n");
	else if(temp->Permission==3)
		printf("File Permission:Read & Write...\n");
	printf("------------------------------------------------------\n");
	return 0;
}




int stat_file(char  *name)
{
	PINODE temp=head;

	if(name==NULL)
		return -1;

	while(temp!=NULL)
	{
		if(strcmp(temp->FileName,name)==0)
			break;
		temp=temp->next;
	}
	if(temp=NULL)
		return -2;

	printf("----------Statistical Information About File----------\n");
	printf("File Name:%s\n",temp->FileName);
	printf("Inode Number:%d\n",temp->InodeNumber);
	printf("File Size:%d\n",temp->FileSize);
	printf("Actual File Size:%d\n",temp->FileActualSize);
	printf("Link Count:%d\n",temp->LinkCount);
	printf("Reference Count:\n",temp->ReferenceCount);
	if(temp->Permission==1)
		printf("File Permission:Read Only...\n");
	else if(temp->Permission==2)
		printf("File Permission:Write Only...\n");
	else if(temp->Permission==3)
		printf("File Permission:Read & Write...\n");
	printf("------------------------------------------------------\n");
	return 0;
}



int GetFDFromName(char *name)
{
	int i=0,fd=-1;
	while(i<50)
	{
		if(UFDTArr[i].ptrfiletable!=NULL)
		{
			if(stricmp((UFDTArr[i].ptrfiletable->ptrinode->FileName),name)==0)
				fd=i;
		}
		i++;
	}
	return fd;
}

int rm_file(char *name)
{
	int fd=-1;
	fd=GetFDFromName(name);
//	if(fd==-1)
//		return -1;
	PINODE temp=NULL;

	temp=Get_Inode(name);
	if(temp==NULL)
		return -1;
	(temp->LinkCount)--;
	if(temp->LinkCount==0)
	{
		temp->FileType=0;
		//free(UFDTArr[fd].ptrfiletable->ptrinode->Buffer);			buffer not freed,for recovery
	}
	//free(UFDTArr[fd].ptrfiletable);
	if(fd!=-1)
		free(UFDTArr[fd].ptrfiletable);

	UFDTArr[fd].ptrfiletable=NULL;
	(SUPERBLOCKobj.Freeinode)++;
}



void CloseFileByName(int fd)
{
	UFDTArr[fd].ptrfiletable->readoffset=0;
	UFDTArr[fd].ptrfiletable->writeoffset=0;
	(UFDTArr[fd].ptrfiletable->ptrinode->ReferenceCount)--;
	if((UFDTArr[fd].ptrfiletable->ptrinode->ReferenceCount)==0)
	{
		free(UFDTArr[fd].ptrfiletable);
		UFDTArr[fd].ptrfiletable=NULL;
		printf("\nFile closed...\n");
	}
}


int CloseFileByName(char *name)
{
	int fd=0;
	fd=GetFDFromName(name);
	if(fd==-1)
		return -1;
	UFDTArr[fd].ptrfiletable->readoffset=0;
	UFDTArr[fd].ptrfiletable->writeoffset=0;
	(UFDTArr[fd].ptrfiletable->ptrinode->ReferenceCount)--;
	if((UFDTArr[fd].ptrfiletable->ptrinode->ReferenceCount)==0)
	{
		free(UFDTArr[fd].ptrfiletable);
		UFDTArr[fd].ptrfiletable=NULL;
		printf("\nFile closed...\n");
	}
	return 0;
}


void CloseAllFile()
{
	int i=0;
	while(i<50)
	{
		if(UFDTArr[i].ptrfiletable!=NULL)
		{
			UFDTArr[i].ptrfiletable->readoffset=0;
			UFDTArr[i].ptrfiletable->writeoffset=0;
			(UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;
		}
		i++;
	}
}



int OpenFile(char*name,int mode)
{
	int i=0;
	PINODE temp=NULL;

	if(name==NULL || mode<=0)
		return -1;


	temp=Get_Inode(name);
	if(temp==NULL)
		return -2;


	if(temp->Permission<mode)
		return 3;

	while(i<50)
	{
		if(UFDTArr[i].ptrfiletable==NULL)
			break;
		i++;
	}
	
	UFDTArr[i].ptrfiletable=(PFILETABLE)malloc(sizeof(FILETABLE));
	if(UFDTArr[i].ptrfiletable==NULL)
		return -1;
	UFDTArr[i].ptrfiletable->count=1;
	UFDTArr[i].ptrfiletable->mode=mode;

	if(mode==READ+WRITE)
	{
		UFDTArr[i].ptrfiletable->readoffset=0;
		UFDTArr[i].ptrfiletable->writeoffset=0;
	}
	else if(mode==READ)
	{
		UFDTArr[i].ptrfiletable->readoffset=0;
	}
	else if(mode==WRITE)
	{
		UFDTArr[i].ptrfiletable->writeoffset=0;
	}

	UFDTArr[i].ptrfiletable->ptrinode=temp;
	(UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)++;


	return i;
}



int WriteFile(int fd,char* arr,int isize)
{
	if((UFDTArr[fd].ptrfiletable->mode!=WRITE)&&(UFDTArr[fd].ptrfiletable->mode!=READ+WRITE))
		return -1;

	if((UFDTArr[fd].ptrfiletable->ptrinode->Permission!=WRITE)&&(UFDTArr[fd].ptrfiletable->ptrinode->Permission!=READ+WRITE))
		return -1;

	if(UFDTArr[fd].ptrfiletable->ptrinode->FileType!=REGULAR)
		return -3;

	if(UFDTArr[fd].ptrfiletable->writeoffset==MAXFILESIZE)
		return -2;

	strncpy((UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->writeoffset),arr,isize);

//	if((UFDTArr[fd].ptrfiletable->writeoffset)+isize)>MAXFILESIZE)		{(UFDTArr[fd].ptrfiletable->writeoffset)=MAXFILESIZE}return length of copied data
	UFDTArr[fd].ptrfiletable->writeoffset=(UFDTArr[fd].ptrfiletable->writeoffset)+isize;

	UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+isize;
	printf("size:%d",isize);
	return isize;
}


int truncate_File(char *name)
{
	int fd=0;
	fd=GetFDFromName(name);
	if(fd==-1)
		return -1;

	memset(UFDTArr[fd].ptrfiletable->ptrinode->Buffer,0,MAXFILESIZE);
	UFDTArr[fd].ptrfiletable->readoffset=0;
	UFDTArr[fd].ptrfiletable->writeoffset=0;
	UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize=0;

	return 0;
}



int ReadFile(int fd,char *arr,int isize)
{
	int r_size=0;
	memset(arr,0,MAXFILESIZE);
	if(UFDTArr[fd].ptrfiletable==NULL)
	return -1;

	if((UFDTArr[fd].ptrfiletable->mode!=READ)&&(UFDTArr[fd].ptrfiletable->mode!=READ+WRITE))
		return -2;
	if((UFDTArr[fd].ptrfiletable->ptrinode->Permission!=READ)&&(UFDTArr[fd].ptrfiletable->ptrinode->Permission!=READ+WRITE))
		return -2;

	if(UFDTArr[fd].ptrfiletable->readoffset==UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
		return -3;


	if(UFDTArr[fd].ptrfiletable->ptrinode->FileType!=REGULAR)
		return -4;

	r_size=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)-(UFDTArr[fd].ptrfiletable->readoffset);
	
	if(r_size<isize)
	{
		strncpy(arr,((UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->readoffset)),r_size);

		UFDTArr[fd].ptrfiletable->readoffset=(UFDTArr[fd].ptrfiletable->readoffset)+r_size;
		return r_size;
	}
	else
	{
		strncpy(arr,((UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->readoffset)),isize);

		UFDTArr[fd].ptrfiletable->readoffset=(UFDTArr[fd].ptrfiletable->readoffset)+isize;
	}

	return isize;
}


int CreateFile(char*name,int permission)
{
	int i=0;
	PINODE temp=head;

	if(name==NULL||permission>3||permission==0)
		return -1;
	if(SUPERBLOCKobj.Freeinode==0)
		return -2;
	if(Get_Inode(name)!=NULL)
		return -3;

	(SUPERBLOCKobj.Freeinode)--;

	while(temp!=NULL)
	{
		if(temp->FileType==0)
			break;
		temp=temp->next;
	}

/*	while(i<50)
	{
		if(UFDTArr[i].ptrfiletable==NULL)
			break;
		i++;
	}

	UFDTArr[i].ptrfiletable=(PFILETABLE)malloc(sizeof(FILETABLE));
	if(UFDTArr[i].ptrfiletable==NULL)
	return -4;

	UFDTArr[i].ptrfiletable->count=1;
	UFDTArr[i].ptrfiletable->mode=permission;
	UFDTArr[i].ptrfiletable->readoffset=0;
	UFDTArr[i].ptrfiletable->writeoffset=0;

	UFDTArr[i].ptrfiletable->ptrinode=temp;*/

	strcpy_s((temp->FileName),name);
	temp->FileType=REGULAR;
	temp->ReferenceCount=0;
	temp->LinkCount=1;
	temp->FileSize=MAXFILESIZE;
	temp->FileActualSize=0;
	temp->Permission=permission;
	if(temp->Buffer!=NULL)
		free(temp->Buffer);
	temp->Buffer=(char*)malloc(MAXFILESIZE);
	memset(temp->Buffer,0,MAXFILESIZE);
//	return i;
//	return(OpenFile(name,permission));
}



int LseekFile(int fd,int size,int from)
{
	if(fd<0 || from>2)
		return -1;
	if(UFDTArr[fd].ptrfiletable==NULL)
		return -1;

	if((UFDTArr[fd].ptrfiletable->mode==READ)||(UFDTArr[fd].ptrfiletable->mode==READ+WRITE))
	{
		if(from==CURRENT)
		{
			if(((UFDTArr[fd].ptrfiletable->readoffset)+size)>(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
				return -1;
			if(((UFDTArr[fd].ptrfiletable->readoffset)+size)<0)
				return -1;
			(UFDTArr[fd].ptrfiletable->readoffset)=(UFDTArr[fd].ptrfiletable->readoffset) + size;
		}

		if(from==START)
		{
			if(size>(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
				return -1;
			if(size<0)
				return -1;
			UFDTArr[fd].ptrfiletable->readoffset=size;
		}

		if(from==END)
		{
			if(((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size)>MAXFILESIZE)
				return -1;
			if(((UFDTArr[fd].ptrfiletable->readoffset)+size)<0)
				return -1;
			UFDTArr[fd].ptrfiletable->readoffset=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size;
		}
	}

	else if((UFDTArr[fd].ptrfiletable->mode==WRITE)||(UFDTArr[fd].ptrfiletable->mode==READ+WRITE))
	{
		if(from==CURRENT)
		{
			if(((UFDTArr[fd].ptrfiletable->writeoffset)+size)>MAXFILESIZE)
				return -1;
			if(((UFDTArr[fd].ptrfiletable->writeoffset)+size)<0)
				return -1;
			if(((UFDTArr[fd].ptrfiletable->writeoffset)+size)>(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
				(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)=((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size);
			UFDTArr[fd].ptrfiletable->writeoffset=(UFDTArr[fd].ptrfiletable->writeoffset)+size;
		}

		if(from==START)
		{
			if(size>MAXFILESIZE)
				return -1;
			if(size<0)
				return -1;
			if(((UFDTArr[fd].ptrfiletable->writeoffset)+size)>(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
				UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize=(UFDTArr[fd].ptrfiletable->writeoffset)+size;
		}

		if(from==END)
		{
			if(((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size)>MAXFILESIZE)
				return -1;
			if(((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size)<0)
				return -1;
			UFDTArr[fd].ptrfiletable->writeoffset=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size;
		}
	}
	printf("pos:%d\n",UFDTArr[fd].ptrfiletable->readoffset);
}



void backup()
{
	PINODE temp=head;
	char arr[MAXFILESIZE],file_name[100];
	int i=0,fd=-1;
	printf("\nRecovering data...\nPlease wait...\n");
	sprintf(file_name,"%s%s",path,fileHandle->d_name);
//	printf("%s",file_name);
	fd=open(file_name,O_RDONLY);
	if(fd<0)
		printf("ERROR:%s",strerror(errno));

	_read(fd,arr,sizeof(arr));

	UFDTArr[i].ptrfiletable=(PFILETABLE)malloc(sizeof(filetable));
		
	UFDTArr[i].ptrfiletable->count=1;
	UFDTArr[i].ptrfiletable->mode=3;//?????????
	UFDTArr[i].ptrfiletable->readoffset=UFDTArr[i].ptrfiletable->writeoffset=0;
	UFDTArr[i].ptrfiletable->ptrinode=temp;

	strcpy_s((UFDTArr[i].ptrfiletable->ptrinode->FileName),fileHandle->d_name);
	UFDTArr[i].ptrfiletable->ptrinode->FileType=REGULAR;//dir?
	UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount=1;
	UFDTArr[i].ptrfiletable->ptrinode->LinkCount=1;
	UFDTArr[i].ptrfiletable->ptrinode->FileSize=MAXFILESIZE;
	UFDTArr[i].ptrfiletable->ptrinode->FileActualSize=strlen(arr);//////////////////////////////////
//	printf("size:%d",UFDTArr[i].ptrfiletable->ptrinode->FileActualSize);
	UFDTArr[i].ptrfiletable->ptrinode->Permission=1;
	UFDTArr[i].ptrfiletable->ptrinode->Buffer=(char*)malloc(MAXFILESIZE);
	memset(UFDTArr[i].ptrfiletable->ptrinode->Buffer,0,MAXFILESIZE);
	strcpy((UFDTArr[i].ptrfiletable->ptrinode->Buffer),arr);
	SUPERBLOCKobj.Freeinode--;
	printf("\n%s\nStatus:Recovered...\n",fileHandle->d_name);

	while((fileHandle=readdir(dirHandle))!=NULL)
	{
		sprintf(file_name,"%s%s",path,fileHandle->d_name);
	//	printf("%s",file_name);
		fd=open(file_name,O_RDONLY);
		if(fd<0)
			printf("ERROR:%s",strerror(errno));
		
		temp=temp->next;//next dilb entry
		i++;//next ufdt entry
		UFDTArr[i].ptrfiletable=(PFILETABLE)malloc(sizeof(filetable));

		UFDTArr[i].ptrfiletable->count=1;
		UFDTArr[i].ptrfiletable->mode=3;//?????????
		UFDTArr[i].ptrfiletable->readoffset=UFDTArr[i].ptrfiletable->writeoffset=0;
		UFDTArr[i].ptrfiletable->ptrinode=temp;

		strcpy_s((UFDTArr[i].ptrfiletable->ptrinode->FileName),fileHandle->d_name);
		UFDTArr[i].ptrfiletable->ptrinode->FileType=REGULAR;//dir?
		UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount=1;
		UFDTArr[i].ptrfiletable->ptrinode->LinkCount=1;
		UFDTArr[i].ptrfiletable->ptrinode->FileSize=MAXFILESIZE;
		UFDTArr[i].ptrfiletable->ptrinode->FileActualSize=_read(fd,arr,sizeof(arr));//////////////////////////////////
		UFDTArr[i].ptrfiletable->ptrinode->Permission=1;
		UFDTArr[i].ptrfiletable->ptrinode->Buffer=(char*)malloc(MAXFILESIZE);
		memset(UFDTArr[i].ptrfiletable->ptrinode->Buffer,0,MAXFILESIZE);
		strcpy((UFDTArr[i].ptrfiletable->ptrinode->Buffer),arr);
		_sleep(1000);
		SUPERBLOCKobj.Freeinode--;
		printf("%s\nStatus:Recovered...\n",fileHandle->d_name);
	}

		
			

}


void recover(char * name)
{
	PINODE temp=head;
	int i=0;
	if(Get_Inode(name))
	{
		printf("\nERROR:Specified file already exists!!!\n");
		return;
	}
	while(temp!=NULL)
	{
		if((temp->FileType==0)&&(stricmp(temp->FileName,name)==0))
			break;
		temp=temp->next;
	}
	printf("%s",name);
	temp->FileType=REGULAR;
	temp->LinkCount=1;
	temp->FileSize=MAXFILESIZE;

	while(i<MAXINODE)
	{
		if(UFDTArr[i].ptrfiletable==NULL)
			break;
	}
	UFDTArr[i].ptrfiletable=(PFILETABLE)malloc(sizeof(filetable));
	if(UFDTArr[i].ptrfiletable==NULL)
	{
		printf("\nERROR:Memory Allocation Failed\n");
		return;
	}
	UFDTArr[i].ptrfiletable->count=1;
	UFDTArr[i].ptrfiletable->mode=3;//?????????
	UFDTArr[i].ptrfiletable->readoffset=UFDTArr[i].ptrfiletable->writeoffset=0;
	UFDTArr[i].ptrfiletable->ptrinode=temp;
	printf("%s successfully recovered in read/write mode!!!",temp->FileName);
}



int main()
{
	
	char *ptr=NULL;
	int ret=0,fd=0,count=0;
	char command[4][80];
	char str[80],arr[1024];
	char ch;
	InitializeSuperBlock();
	CreateDILB();
	dirHandle=opendir(path);
	if(dirHandle!=NULL)
	{
		readdir(dirHandle);
		readdir(dirHandle);
		if((fileHandle=readdir(dirHandle))!=NULL)
		{
			printf("Marvellous VFS:>");
			printf("Do you want to recover previous data?y/n:");
			scanf("%c",&ch);
			if(ch=='y')
				backup();
		}
	}
	

	while(1)
	{
		fflush(stdin);
		memset(str,0,sizeof(str));
		printf("Marvellous VFS:>");
		fgets(str,80,stdin);

		count=sscanf(str,"%s%s%s%s",command[0],command[1],command[2],command[3]);


		if(count==1)
		{
			if(stricmp(command[0],"ls")==0)
			{
				ls_file();
				continue;
			}
			else if(stricmp(command[0],"clear")==0)
			{
				fflush(stdin);
				system("cls");
				continue;
			}
			else if(stricmp(command[0],"closeall")==0)
			{
				CloseAllFile();
				printf("All files closed succcessfully\n");
				continue;
			}
			else if(stricmp(command[0],"exit")==0)
			{
				int fd=0,x=0;
				char ans;
				char addr[]="C:\\Users\\LENOVO\\Desktop\\vfs\\";
				char str[100];
				printf("Do you want to save???y/n\n");
				scanf("%c",&ans);
				if(ans=='y')
				{
					PINODE temp=head;
					int i=0;
					while(i<MAXINODE)
					{
						if(temp->FileType!=0)
						{
							sprintf(str,"%s%s",addr,temp->FileName);
							printf("\n%s\n",str);
						//	fd=_creat(str,0777);
					//		if(fd<0)
					//			printf("%s",strerror(errno));
							fd=_open(str,O_WRONLY | O_CREAT);
							if(fd<0){printf("%s",strerror(errno));}
							x=_write(fd,temp->Buffer,MAXFILESIZE);//<-
							if(!x)
							printf("%s",strerror(errno));
							close(fd);
						}
						temp=temp->next;
						i++;
					}
				}
				else if(ans=='n')
				printf("\nDONE\n");
				printf("Terminating Marvellous Virtual File System...\n");
				_sleep(1000);
				printf("Closing all files...\n");
				_sleep(300);
				printf("Done\n");
				printf("Clearing all data...\n");
				_sleep(1000);
				printf("Done\n");
				_sleep(500);
				printf("Done\n");
				break;
			}
			else if(stricmp(command[0],"help")==0)
			{
				DisplayHelp();
				continue;
			}
			else
			{
				printf("\nERROR:Command not found!!!\n");
				continue;
			}
		}
		else if(count==2)
		{
			if(stricmp(command[0],"recover")==0)
			{
				recover(command[1]);
				continue;
			}
			if(stricmp(command[0],"stat")==0)
			{
				ret=stat_file(command[1]);
				if(ret==-1)
					printf("ERROR:Incorrect parameters..\n");
				if(ret==-2)
					printf("ERROR:There is no such file..\n");
				continue;
			}
			else if(stricmp(command[0],"fstat")==0)
			{
				ret=fstat_file(atoi(command[1]));
				if(ret==-1)
					printf("ERROR:Incorrect parameters..\n");
				if(ret==-2)
					printf("ERROR:There is no such file..\n");
				continue;
			}
			else if(stricmp(command[0],"close")==0)
			{
				ret=CloseFileByName(command[1]);
				if(ret==-1)
				printf("ERROR:There is no such file\n");
				continue;
			}
			else if(stricmp(command[0],"rm")==0)
			{
				ret=rm_file(command[1]);	
				if(ret==-1)
				printf("ERROR:There is no such file\n");
				continue;
			}
			else if(stricmp(command[0],"man")==0)
			{
				man(command[1]);
			}
			else if(stricmp(command[0],"write")==0)
			{
				fd=GetFDFromName(command[1]);
				if(fd==-1)
				{
					printf("Permission denied!!!\n");
					continue;
				}
				printf("Enter the data:");
				scanf("%[^'\n']s",arr);
				ret=strlen(arr);
				if(ret==0)
				{
					printf("Invalid parameter\n");
					continue;
				}
				ret=WriteFile(fd,arr,ret);
				if(ret==-1)
				{
					printf("write:Permission denied\n");
				}
				if(ret==-2)
				{
					printf("\nERROR:There is no sufficient memory to write");
				}
				if(ret==-3)
				{
					printf("\nERROR:It is not a regular file\n");
				}
				if(ret>=0)
					printf("\nWriting complete...\n");
			}
			else if(stricmp(command[0],"truncate")==0)
			{
				ret=truncate_File(command[1]);
				if(ret==-1)
					printf("\nERROR:Permission Denied::");
				
			}
			else
				printf("\nERROR:Command not found!!!\n");


		}
		else if(count==3)
		{
			if(stricmp(command[0],"create")==0)
			{
				ret=CreateFile(command[1],atoi(command[2]));
				if(ret>=0)
					printf("File successfully created with file descriptor:%d\n",ret);
				else if(ret==-1)
					printf("ERROR:Incorrect Parameters...\n");
				else if(ret==-2)
					printf("ERROR:No free inodes...\n");
				else if(ret==-3)
					printf("ERROR:File already exists...\n");
				else if(ret==-4)
					printf("ERROR:Memory allocation failure...\n");
				continue;
		}
			else if(stricmp(command[0],"open")==0)
			{
				printf("---%d---",atoi(command[2]));
				ret=OpenFile(command[1],atoi(command[2]));
				if(ret>=0)
					printf("File successfully opened with file desriptor:%d\n",ret);
				else if(ret==-1)
					printf("\nERROR:Incorrect Parameters...\n");
				else if(ret==-2)
					printf("\nERROR:File not Present...\n");
				else if(ret==-3)
					printf("\nERROR:Permissions denied...\n");
				continue;
			}
			else if(stricmp(command[0],"read")==0)
			{
				fd=GetFDFromName(command[1]);
				
				if(fd==-1)
				{
					printf("\nERROR:Incorrect Parameter...\n");
					continue;
				}
				ptr=(char*)malloc(sizeof(atoi(command[2]))+1);
				
				if(ptr==NULL)
				{
					printf("\nERROR:Memory allocation failure...\n");
					continue;
				}
				ret=ReadFile(fd,ptr,atoi(command[2]));
				if(ret==-1)
					printf("\nERROR:File not existing...\n");
				else if(ret==-2)
					printf("\nERROR:Permission denied...\n");
				else if(ret==-3)
					printf("\nERROR:Reached at end of file...\n");
				else if(ret==-4)
					printf("\nERROR:Not a regular file...\n");
				else if(ret>=0)
				{
					//printf("%s",ptr);
					_write(1,ptr,ret);		//writing in stdout file->monitor
					printf("\n");
				}
				continue;
			}
			else
			{
				printf("\nERROR:Command not found!!!\n");
				continue;
			}
		}
		else if(count==4)
		{
			if(stricmp(command[0],"lseek")==0)
			{
				fd=GetFDFromName(command[1]);
				if(fd==-1)
				{
					printf("\nERROR:Incorrect Parameter...\n");
					continue;
				}
				ret=LseekFile(fd,atoi(command[2]),atoi(command[3]));
				if(ret==-1)
				{
					printf("\nERROR:Unable to perform lseek...\n");
					continue;
				}
			}
			else
			{
				printf("\nERROR:Command not found!!!\n");
				continue;
			}
		}
		else
			{
				printf("\nERROR:Command not found!!!\n");
				continue;
			}
	}
	return 0;
}
