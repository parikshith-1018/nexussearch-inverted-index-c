#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <math.h>

#define TABLE_SIZE 101
#define WORD_SIZE 50
#define MAX_DOCS 100
#define MAX_TERMS 20

char* stopWords[] = {"the","is","in","at","of","on","and","a","to"};
int stopCount = 9;

int isStopWord(char* word) {
    for(int i=0;i<stopCount;i++)
        if(strcmp(word, stopWords[i])==0)
            return 1;
    return 0;
}

typedef struct Location {
    int doc_id, paragraph, line, word_no;
    struct Location* next;
} Location;

typedef struct AVLNode {
    char word[WORD_SIZE];
    Location* locations;
    int doc_freq;
    int height;
    struct AVLNode *left,*right;
} AVLNode;

AVLNode* hashTable[TABLE_SIZE];
int totalDocs = 0;

int max(int a,int b){return a>b?a:b;}
int height(AVLNode* n){return n?n->height:0;}

AVLNode* createNode(char* word, Location* loc){
    AVLNode* n = malloc(sizeof(AVLNode));
    strcpy(n->word,word);
    n->locations = loc;
    n->doc_freq = 1;
    n->left=n->right=NULL;
    n->height=1;
    return n;
}

Location* createLocation(int d,int p,int l,int w){
    Location* loc = malloc(sizeof(Location));
    loc->doc_id=d; loc->paragraph=p; loc->line=l; loc->word_no=w;
    loc->next=NULL;
    return loc;
}

AVLNode* rightRotate(AVLNode* y){
    AVLNode* x=y->left;
    AVLNode* t=x->right;
    x->right=y; y->left=t;
    y->height=max(height(y->left),height(y->right))+1;
    x->height=max(height(x->left),height(x->right))+1;
    return x;
}

AVLNode* leftRotate(AVLNode* x){
    AVLNode* y=x->right;
    AVLNode* t=y->left;
    y->left=x; x->right=t;
    x->height=max(height(x->left),height(x->right))+1;
    y->height=max(height(y->left),height(y->right))+1;
    return y;
}

int getBalance(AVLNode* n){return n?height(n->left)-height(n->right):0;}

int hash(char* word){
    int h=0;
    for(int i=0;word[i];i++) h=(h*31+word[i])%TABLE_SIZE;
    return h;
}

AVLNode* insertAVL(AVLNode* node,char* word,Location* loc){
    if(!node) return createNode(word,loc);

    int cmp=strcmp(word,node->word);

    if(cmp==0){
        loc->next=node->locations;
        node->locations=loc;
        node->doc_freq++;
        return node;
    }
    else if(cmp<0) node->left=insertAVL(node->left,word,loc);
    else node->right=insertAVL(node->right,word,loc);

    node->height=1+max(height(node->left),height(node->right));

    int bal=getBalance(node);

    if(bal>1 && strcmp(word,node->left->word)<0) return rightRotate(node);
    if(bal<-1 && strcmp(word,node->right->word)>0) return leftRotate(node);
    if(bal>1 && strcmp(word,node->left->word)>0){ node->left=leftRotate(node->left); return rightRotate(node);}    
    if(bal<-1 && strcmp(word,node->right->word)<0){ node->right=rightRotate(node->right); return leftRotate(node);}    

    return node;
}

void insert(char* word,int d,int p,int l,int w){
    if(isStopWord(word)) return;
    int idx=hash(word);
    Location* loc=createLocation(d,p,l,w);
    hashTable[idx]=insertAVL(hashTable[idx],word,loc);
}

AVLNode* searchAVL(AVLNode* root,char* word){
    if(!root) return NULL;
    int cmp=strcmp(word,root->word);
    if(cmp==0) return root;
    else if(cmp<0) return searchAVL(root->left,word);
    else return searchAVL(root->right,word);
}

float computeTFIDF(AVLNode* node){
    float tf = node->doc_freq;
    float idf = log((float)totalDocs/(1+node->doc_freq));
    return tf * idf;
}

void normalize(char* w){ for(int i=0;w[i];i++) w[i]=tolower(w[i]); }

void processFile(char* filename,int doc_id){
    FILE* fp=fopen(filename,"r"); if(!fp) return;

    char line[256]; int para=1,line_no=0;

    while(fgets(line,sizeof(line),fp)){
        if(strcmp(line,"\n")==0){para++;continue;}
        line_no++; int word_no=0;

        char* token=strtok(line," ,.-\n");
        while(token){
            word_no++; normalize(token);
            insert(token,doc_id,para,line_no,word_no);
            token=strtok(NULL," ,.-\n");
        }
    }
    fclose(fp);
}

void indexDirectory(char* path){
    struct dirent* entry;
    DIR* dir=opendir(path);
    if(!dir){ printf("Directory error\n"); return; }

    int doc_id=1;
    while((entry=readdir(dir))){
        if(strstr(entry->d_name,".txt")){
            char full[256];
            sprintf(full,"%s/%s",path,entry->d_name);
            processFile(full,doc_id++);
        }
    }
    totalDocs = doc_id-1;
    closedir(dir);
}

int matchPosition(Location* l1, Location* l2) {
    return (l1->doc_id == l2->doc_id &&
            l1->paragraph == l2->paragraph &&
            l1->line == l2->line &&
            l2->word_no == l1->word_no + 1);
}

void phraseSearch(char terms[][WORD_SIZE], int n) {
    AVLNode* nodes[MAX_TERMS];

    for(int i=0;i<n;i++){
        int idx = hash(terms[i]);
        nodes[i] = searchAVL(hashTable[idx], terms[i]);
        if(!nodes[i]){
            printf("Phrase not found\n");
            return;
        }
    }

    printf("\nPhrase results:\n");

    Location* base = nodes[0]->locations;

    while(base){
        int found = 1;
        Location* prev = base;

        for(int i=1;i<n;i++){
            Location* curr = nodes[i]->locations;
            int matched = 0;

            while(curr){
                if(matchPosition(prev, curr)){
                    matched = 1;
                    prev = curr;
                    break;
                }
                curr = curr->next;
            }

            if(!matched){ found = 0; break; }
        }

        if(found){
            printf("Doc:%d Para:%d Line:%d StartWord:%d\n",
                   base->doc_id, base->paragraph, base->line, base->word_no);
        }

        base = base->next;
    }
}

int main(){
    indexDirectory("docs");

    char query[200];

    while(1){
        printf("\nEnter query (use quotes for phrase): ");
        fgets(query,200,stdin);
        query[strcspn(query,"\n")] = 0;

        if(strcmp(query,"exit")==0) break;

        if(query[0] == '"'){
            char* end = strrchr(query, '"');
            if(end){
                *end = '\0';
                char* phrase = query+1;

                char terms[MAX_TERMS][WORD_SIZE];
                int count = 0;

                char* tok = strtok(phrase, " ");
                while(tok && count < MAX_TERMS){
                    normalize(tok);
                    strcpy(terms[count++], tok);
                    tok = strtok(NULL, " ");
                }

                phraseSearch(terms, count);
            }
        }
        else{
            char* tok = strtok(query, " ");
            while(tok){
                normalize(tok);
                int idx = hash(tok);
                AVLNode* node = searchAVL(hashTable[idx], tok);
                if(node)
                    printf("%s -> Score: %.3f\n", tok, computeTFIDF(node));
                else
                    printf("%s -> Not found\n", tok);
                tok = strtok(NULL, " ");
            }
        }
    }

    return 0;
}
