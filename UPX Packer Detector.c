#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shlwapi.lib")

#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <stdio.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <stddef.h>
#include <limits.h>
#include <malloc.h>
#include "UPXSignatures.h"
#include "resource.h"

// g (Global optimization), s (Favor small code), y (No frame pointers).
#pragma optimize("gsy", on)
#pragma comment(linker, "/FILEALIGN:0x200")
#pragma comment(linker, "/MERGE:.rdata=.data")
#pragma comment(linker, "/MERGE:.text=.data")
#pragma comment(linker, "/MERGE:.reloc=.data")
#pragma comment(linker, "/SECTION:.text, EWR /IGNORE:4078")
#pragma comment(linker, "/OPT:NOWIN98")		// Make section alignment really small.
#define WIN32_LEAN_AND_MEAN

BOOL		CALLBACK Main(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
void		InitList(void);
void		GetFolderName(HWND hwndOwner, HWND hwndTextBox);
BOOL		SelectFolder(LPTSTR pszSelectedDir,LPTSTR szCaption,HWND hOwner);
BOOL		ScanIt(char *szFilePath);
void		ScanDir(char *szDirPath);

typedef struct{
	char	*sign;
	char	*name;
	int		len;
	void	*next;
} UPX_sign;

HINSTANCE				hInst, hCheck;
HWND					hDlg, hwndList;
IMAGE_DOS_HEADER		dh;
IMAGE_NT_HEADERS		nth;
IMAGE_SECTION_HEADER	*Sections;
long					PE_offs, Sct_offs, EP;
char					*szBuff;
LVITEM					lvItem;
int						i, iSelect, iSects, iNumFiles= 0, iIndex= 0;

int WINAPI WinMain(HINSTANCE hinstCurrent,HINSTANCE hinstPrevious,LPSTR lpszCmdLine,int nCmdShow){
	INITCOMMONCONTROLSEX icc;
	icc.dwICC= ICC_LISTVIEW_CLASSES;
	icc.dwSize= sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&icc);
	hInst= hinstCurrent;
	DialogBox(hinstCurrent, MAKEINTRESOURCE(IDD_MAIN), NULL, (DLGPROC)Main);
	return 0;
}

BOOL CALLBACK Main(HWND hDlgMain,UINT uMsg,WPARAM wParam,LPARAM lParam){
	HDC					hdc;
	PAINTSTRUCT			ps;
	HRESULT				hRes;
	OPENFILENAME		ofn;
	int					iCheck;
	char				filename[512]= "";
	char				*szFilePath, *szCommand, *szParameters, *szFName, *szPN;
	HANDLE				filehandle= NULL;
	LPNMHDR				lpnmhdr;
	LPNMITEMACTIVATE	lpnmitem;
	LVHITTESTINFO		lvHti;
	HMENU				hMenu;
	POINT				pt;

	hDlg= hDlgMain;

	switch( uMsg ){
		case WM_INITDIALOG:
			SendMessageA(hDlg,WM_SETICON,ICON_SMALL, (LPARAM) LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON)));
			SendMessageA(hDlg,WM_SETICON, ICON_BIG,(LPARAM) LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON)));
			SendMessage(GetDlgItem(hDlg, IDC_RADIOFILE), BM_SETCHECK, BST_CHECKED, 0);
			hwndList= GetDlgItem(hDlg, IDC_LIST);
			ListView_SetExtendedListViewStyle(hwndList, LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);
			InitList();
			szBuff= (char*)calloc(1024, sizeof(char));
		return TRUE;
		case WM_NOTIFY:
			lpnmhdr= (LPNMHDR)lParam;
			if( lpnmhdr->hwndFrom==hwndList ){
				if(lpnmhdr->code == NM_RCLICK) {
					lpnmitem= (LPNMITEMACTIVATE)lParam;
					hMenu= CreatePopupMenu();

					ZeroMemory(&lvHti, sizeof(LVHITTESTINFO));

					lvHti.pt= lpnmitem->ptAction;
					iSelect= ListView_HitTest(hwndList, &lvHti);

					if( lvHti.flags&LVHT_ONITEM ){
						AppendMenu(hMenu, MF_STRING, IDM_UNPACK, "Unpack");
					}
					AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
					GetCursorPos(&pt);
					TrackPopupMenu(hMenu, TPM_LEFTALIGN, pt.x, pt.y, 0, hDlg, 0);
				}
			}
			DestroyMenu(hMenu);
		return TRUE;
		case WM_COMMAND:
			if( LOWORD(wParam)==IDC_EXIT ){
				if( HIWORD(wParam)==BN_CLICKED ){
					EndDialog(hDlg, wParam);
				}
			}else if( LOWORD(wParam)==IDC_CHECKFILE ){
				if( HIWORD(wParam)==BN_CLICKED ){
					ZeroMemory(&ofn,sizeof(ofn));
					ofn.lStructSize= sizeof(OPENFILENAME);
					ofn.hwndOwner= hDlg;
					ofn.lpstrFilter= "All files (*.*)\0*.*\0";
					ofn.lpstrFile= filename;
					ofn.nMaxFile= MAX_PATH;
					ofn.Flags= OFN_EXPLORER|OFN_PATHMUSTEXIST;

					if( GetOpenFileName(&ofn)==TRUE ){
						if( filehandle!=NULL ){
							CloseHandle(filehandle);
							filehandle= NULL;
						}
						filehandle= CreateFile(filename,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
						if( filehandle==INVALID_HANDLE_VALUE ){
							MessageBox(hDlg,"Could not open the selected file.","Error accessing file",MB_OK);
						}else{
							SetDlgItemText(hDlg,IDC_FILEPATH,filename);
						}
					}
				}
			}else if( LOWORD(wParam)==IDC_CHECKDIR ){
				if( HIWORD(wParam)==BN_CLICKED ){
					GetFolderName(hDlg, GetDlgItem(hDlg, IDC_DIRPATH));
				}
			}else if( LOWORD(wParam)==IDC_SCAN ){
				if( HIWORD(wParam)==BN_CLICKED ){
					iCheck= (int) SendMessage(GetDlgItem(hDlg, IDC_RADIOFILE), BM_GETCHECK, 0, 0);
					szFilePath= (char*)calloc(512, sizeof(char));
					if( iCheck ){
						GetWindowText(GetDlgItem(hDlg, IDC_FILEPATH), szFilePath, 512);
						hRes= ScanIt(szFilePath);
						if( hRes ){
							ListView_DeleteAllItems(hwndList);
							lvItem.mask= LVIF_TEXT;
							lvItem.cchTextMax= MAX_PATH;
							lvItem.iItem= iIndex;
							lvItem.iSubItem= 0;
							lvItem.pszText= szFilePath;
							ListView_InsertItem(hwndList, &lvItem);

							lvItem.iItem= iIndex;
							lvItem.iSubItem= 1;
							lvItem.pszText= szBuff;
							ListView_SetItem(hwndList, &lvItem);
						}else{
							MessageBox(hDlg, "Nope, the file is not packed with UPX.", "Not Packed with UPX", MB_OK);
						}
					}else{
						GetWindowText(GetDlgItem(hDlg, IDC_DIRPATH), szFilePath, 2048);
						ScanDir(szFilePath);
					}
				}
			}else if( LOWORD(wParam)==IDC_RADIOFILE ){
				SendMessage(GetDlgItem(hDlg, IDC_RADIODIR), BM_SETCHECK, 0,0);
				SetDlgItemText(hDlg, IDC_DIRPATH, "");
				SendDlgItemMessage(hDlg, IDC_SCANSUBFOLDERS, BM_SETCHECK, (WPARAM)BST_UNCHECKED, (LPARAM)0);
				SendDlgItemMessage(hDlg, IDC_GENREPORT, BM_SETCHECK, (WPARAM)BST_UNCHECKED, (LPARAM)0);
				EnableWindow(GetDlgItem(hDlg,IDC_CHECKFILE), TRUE);
				EnableWindow(GetDlgItem(hDlg,IDC_CHECKDIR), FALSE);
				EnableWindow(GetDlgItem(hDlg,IDC_GENREPORT), FALSE);
				EnableWindow(GetDlgItem(hDlg,IDC_SCANSUBFOLDERS), FALSE);
				EnableWindow(GetDlgItem(hDlg,IDC_SCANFILES), FALSE);
			}else if( LOWORD(wParam)==IDC_RADIODIR ){
				SendMessage(GetDlgItem(hDlg, IDC_RADIOFILE), BM_SETCHECK, 0,0);
				SetDlgItemText(hDlg, IDC_FILEPATH, "");
				EnableWindow(GetDlgItem(hDlg,IDC_CHECKFILE), FALSE);
				EnableWindow(GetDlgItem(hDlg,IDC_CHECKDIR), TRUE);
				EnableWindow(GetDlgItem(hDlg,IDC_GENREPORT), TRUE);
				EnableWindow(GetDlgItem(hDlg,IDC_SCANSUBFOLDERS), TRUE);
				EnableWindow(GetDlgItem(hDlg,IDC_SCANFILES), TRUE);
			}else if( LOWORD(wParam)==IDM_UNPACK ){
				char	*buffer;

				buffer= (char*)calloc(1024, sizeof(char));
				ListView_GetItemText(hwndList, iSelect, 0, buffer, MAX_PATH);
				szCommand= (char*)calloc(1024, sizeof(char));
				szFName= (char*)calloc(2048, sizeof(char));
				szPN= (char*)calloc(2048, sizeof(char));
				szParameters= (char*)calloc(2048, sizeof(char));
				sprintf(szFName, "%s", buffer);
				PathStripPath(szFName);
				GetModuleFileName(NULL, szCommand, 2048);
				strcpy(szPN, szCommand);
				PathRemoveFileSpec(szPN);
				sprintf(szCommand, "upx.exe", szPN);
				sprintf(szParameters, " -d -o \"%s\\unpacked_%s\" \"%s\"", szPN, szFName, buffer);
				hCheck= ShellExecute(NULL, "open", szCommand, szParameters, NULL, SW_SHOW);
				if( hCheck>32 ){
					MessageBox(hDlg, "Unpacking is successful.", "Unpacked Successfully.", MB_OK);
				}
			}
		break;
		case WM_PAINT:
			hdc= BeginPaint(hDlg, &ps);
			InvalidateRect(hDlg, NULL, TRUE);
			EndPaint(hDlg, &ps);
		break;
		case WM_CLOSE:
			EndDialog(hDlg,wParam);
			DestroyWindow(hDlg);
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
		break;
	}
	return FALSE;
}

void ScanDir(char *szDirPath){
	WIN32_FIND_DATA	wfd;
	HRESULT			hRes;
	HANDLE			hFirstFile;
	char			szBuffer[MAX_PATH]= "";
	char			*szFilePath, *szData;
	int				iFiles=0, i= 0;

	// Format szBuffer to append both values.
	sprintf(szBuffer, "%s\\%s", szDirPath, "*.*"); 

	// Get first file of the current directory.
	hFirstFile= FindFirstFile(szBuffer, &wfd);
	
	// Check to see if hFirstFile is valid.
	if( hFirstFile==INVALID_HANDLE_VALUE ){
		return;
	}
	ListView_DeleteAllItems(hwndList);
	// Begin loop to log all files in specified directory.
	do{
		szData= (char *)calloc(2048, sizeof(char));
		szFilePath= (char *)calloc(2048, sizeof(char));
		// Format szFilePath to append both values
		sprintf(szFilePath, "%s\\%s", szDirPath, wfd.cFileName);
		if( ( (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==FILE_ATTRIBUTE_DIRECTORY ) && ( wfd.cFileName[0]!='.' ) ){
			ScanDir(szFilePath);
		}else if( ( wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY )!=FILE_ATTRIBUTE_DIRECTORY ){
			hRes= ScanIt(szFilePath);
			if( hRes ){
				//ListView_DeleteAllItems(hwndList);
				lvItem.mask= LVIF_TEXT;
				lvItem.cchTextMax= MAX_PATH;
				lvItem.iItem= iIndex;
				lvItem.iSubItem= 0;
				lvItem.pszText= szFilePath;
				ListView_InsertItem(hwndList, &lvItem);

				lvItem.iItem= iIndex;
				lvItem.iSubItem= 1;
				lvItem.pszText= szBuff;
				ListView_SetItem(hwndList, &lvItem);
				iIndex++;
			}
			iFiles++;
		}
		free(szFilePath);
		szFilePath= NULL;
	}while( FindNextFile(hFirstFile, &wfd) );
	FindClose(hFirstFile);
}

BOOL ScanIt(char *szFilePath){
	FILE			*fp;
	BOOL			bRes= FALSE;
	UPX_sign		*sign_list= NULL, *tp, *tp1;
	char			*filepath;
	int				tb= 0, tbd= 0;
	unsigned char	c, c1, c2;

	filepath= szFilePath;

	if( (fp= fopen(filepath, "r"))==NULL ){
		MessageBox(hDlg,"Could not open the selected file.","Error accessing file",MB_OK);
	}else{
		fread(&dh, sizeof(dh), 1, fp);
		PE_offs= dh.e_lfanew;
		fseek(fp, PE_offs, SEEK_SET);
		fread(&nth, sizeof(nth), 1, fp);
		Sct_offs = PE_offs + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + sizeof(IMAGE_OPTIONAL_HEADER);
		Sct_offs -= sizeof(IMAGE_DATA_DIRECTORY) * IMAGE_NUMBEROF_DIRECTORY_ENTRIES;
		Sct_offs += sizeof(IMAGE_DATA_DIRECTORY) * nth.OptionalHeader.NumberOfRvaAndSizes;
		iSects = nth.FileHeader.NumberOfSections;
		Sections= (IMAGE_SECTION_HEADER *)malloc(iSects * sizeof(IMAGE_SECTION_HEADER));
		EP= nth.OptionalHeader.AddressOfEntryPoint;
		
		fseek(fp, Sct_offs, SEEK_SET);
		for( i=0; i<iSects; i++ ){
			char sct_str[9];
			fread(&Sections[i], sizeof(IMAGE_SECTION_HEADER), 1, fp);
			strncpy(sct_str, Sections[i].Name, 8); sct_str[8] = '\0';

			if( (EP>=Sections[i].VirtualAddress)&&(EP<Sections[i].VirtualAddress+Sections[i].Misc.VirtualSize) ){
				EP -= Sections[i].VirtualAddress - Sections[i].PointerToRawData;
			}
		}
		fseek(fp, EP, SEEK_SET);
		// Create signature list.
		for(i=0; i<sizeof(upx_signatures)/sizeof(char*); i+=2){
			if(!sign_list){
				sign_list= malloc(sizeof(UPX_sign));
				tp= sign_list;
			}else{
				tp1= tp;
				tp= malloc(sizeof(UPX_sign));
				tp1->next= tp;
			}
			tp->sign= upx_signatures[i];
			tp->name= upx_signatures[i+1];
			tp->len= strlen(tp->sign);
			tp->next= NULL;
		}
		tp= sign_list;
		tp1= NULL;
		
		i=0;
		while( sign_list ){
			c= fgetc(fp);
			c1= c & 0x0F;
			c2= (c & (0xF0)) >> 4;
			c1 += (c1 < 10) ? '0' : ('A'-10);
			c2 += (c2 < 10) ? '0' : ('A'-10);
			
			tp= sign_list;
			tp1= NULL;
			
			while( tp ){
				// printf("%s == %s\n", tp->name, tp->sign);							
				tbd= 0;
				
				if( ((tp->sign[i*2] != '?') && (c2 != tp->sign[i*2])) || ((tp->sign[i*2+1] != '?') && (c1 != tp->sign[i*2+1])) ){
					tbd= 1;
				}else{ // signature found
					if( (i+1)*2 == tp->len ){
						sprintf(szBuff, "%s", tp->name);
						bRes= TRUE;
						//printf("  %s\n", tp->name);
						tbd= 1;
						tb= 1;
					}
				}								
				if(tbd){ // delete signature from list
					if(!tp1){
						tp= sign_list;
						sign_list= sign_list->next;
						free(tp);
						tp= sign_list;
					}else{
						tp= tp->next;
						free(tp1->next);
						tp1->next= tp;
					}
				}else{
					tp1= tp;
					tp= tp->next;
				}								
			}
			i++;
		}
		if(!tb){
			bRes= FALSE;;
		}
	}
	fclose(fp);
	return bRes;
}

void InitList( void ){
	LVCOLUMN lvCol;
	char *szColumn[]= {"FileName", "Packer"};
	int i, width[]= {150, 150};

	ZeroMemory(&lvCol, sizeof(LVCOLUMN));

	lvCol.mask= LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH | LVCF_FMT;
	lvCol.fmt= LVCFMT_LEFT;

	for( i=0; i<2; i++ ){
		lvCol.iSubItem= i;
		lvCol.cx= width[i];
		lvCol.pszText= szColumn[i];

		ListView_InsertColumn(hwndList, i, &lvCol);
	}
}

// get folder path into a text box.
void GetFolderName(HWND hwndOwner,HWND hwndTextBox){
	char pstrDir[MAX_PATH];

	// select a folder
	if (SelectFolder(pstrDir,NULL,hwndOwner)==TRUE){
		SetWindowText(hwndTextBox, pstrDir);// if folder path is selected then set it to text box.
	}
}

BOOL SelectFolder(LPTSTR pszSelectedDir,LPTSTR szCaption,HWND hOwner){
    //The BROWSEINFO struct Contains parameters for the SHBrowseForFolder function .
	BROWSEINFO      browseInfo;
    BOOL            bValid= FALSE;
	LPITEMIDLIST	pIDL;
	
	//fill browseInfo (BROWSEINFO struct) with zeros
	ZeroMemory(&browseInfo, sizeof(BROWSEINFO));

    browseInfo.ulFlags=  BIF_RETURNFSANCESTORS;
	browseInfo.hwndOwner= hOwner;
	browseInfo.lpszTitle= szCaption;
	
	// show  the browse for folder dialog box 
	pIDL = SHBrowseForFolder(&browseInfo);

	if( pIDL!=NULL ){
		// get the path of selected folder  
		if( (SHGetPathFromIDList(pIDL,pszSelectedDir))!=0 ){
            bValid= TRUE;
		}
		//free the item id list allocated by SHBrowseForFolder
		CoTaskMemFree(pIDL);
    }
	return(bValid);
}