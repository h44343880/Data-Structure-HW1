#include<stdio.h>
#include<stdlib.h>

typedef struct Node{
    int id;
    int mem_cnt;
    int * channel;
}Node;

int main(){
    // 讀入資料
    int nodes, links, requests;
    scanf("%d%d%d", &nodes, &links, &requests);
    Node * node_arr = (Node*)malloc(sizeof(Node) * nodes);// 建立儲存所有node資訊的array
    int id, mem_cnt;
    for(int i = 0; i < nodes; i++){
        scanf("%d%d", &id, &mem_cnt);
        node_arr[id].mem_cnt = mem_cnt;
        node_arr[id].channel = (int*)calloc(nodes,sizeof(int));
    }
    int first, second, cnt;
    for(int i = 0; i < links; i++){
        scanf("%d%d%d%d", &id, &first, &second, &cnt);
        node_arr[first].channel[second] = cnt;
        node_arr[second].channel[first] = cnt; 
    }
    int * req_start = (int*)malloc(sizeof(int) * requests);
    int * req_end = (int*)malloc(sizeof(int) * requests);
    for(int i = 0; i < requests; i++){
        scanf("%d%d%d", &id, &req_start[i], &req_end[i]);
    }

    int * queue = (int*)malloc(sizeof(int) * nodes);
    int front, back;

    char * color_arr = (char*)malloc(sizeof(char) * nodes);
    int * predecessor_arr = (int*)malloc(sizeof(int) * nodes);

    int ** path_arr = (int**)calloc(requests, sizeof(int*));// 將可以用的path存起來
    int * accept_arr = (int*)calloc(requests, sizeof(int));// 紀錄那些request為accepted
    int accept_cnt = 0;// 紀錄accept request的數量    

    for(int i = 0; i < requests; i++){
        // printf("%dth run\n", i);
        // 初始化
        front = -1;
        back = -1;
        for(int j = 0; j < nodes; j++){
            color_arr[j] = 'w';
            predecessor_arr[j] = -1;
        }
        // 執行bfs找出shortest path
        queue[0] = req_start[i];
        color_arr[queue[0]] = 'g';
        back++;
        while(front != back){
            for(int j = 0; j < nodes; j++){
                if(node_arr[queue[front + 1]].channel[j]){
                    if(color_arr[j] == 'w'){
                        color_arr[j] = 'g';
                        predecessor_arr[j] = queue[front + 1];
                        queue[++back] = j;
                    }
                }
            }
            color_arr[queue[front + 1]] = 'b';
            front++;
        }
        if(predecessor_arr[req_end[i]] != -1){
            int path_len = 1;
            int current = req_end[i];
            while(current != req_start[i]){// 計算path的長度
                path_len++;
                current = predecessor_arr[current];
            }
            int * path = (int*)malloc(sizeof(int) * nodes);
            for(int j = 0; j < nodes; j++){
                path[j] = -1;
            }
            // 檢查shortest path可不可行
            int accept = 1;
            current = predecessor_arr[req_end[i]];// end的前一個node
            if(node_arr[req_end[i]].mem_cnt == 0 || node_arr[req_start[i]].mem_cnt == 0){
                accept = 0;
                printf("%d:front\n", i);
            }else{
                while(predecessor_arr[current] != -1){
                    if(node_arr[current].mem_cnt < 2){
                        accept = 0;
                        printf("%d:back, cnt: %d, id: %d\n", i, node_arr[current].mem_cnt, current);
                        break;
                    }
                    current = predecessor_arr[current];
                }
            }
            if(accept){
                accept_arr[i] = 1;
                accept_cnt++;
                current = req_end[i];
                path[path_len - 1] = current;
                while(current != req_start[i]){
                    node_arr[current].mem_cnt--;
                    node_arr[predecessor_arr[current]].mem_cnt--;
                    node_arr[current].channel[predecessor_arr[current]]--;
                    node_arr[predecessor_arr[current]].channel[current]--;
                    current = predecessor_arr[current];
                    path_len--;
                    path[path_len - 1] = current;
                }
                path_arr[i] = path;  
            }
        }
    }
    // 輸出結果
    printf("%d\n", accept_cnt);
    for(int i = 0; i < requests; i++){
        if(accept_arr[i]){
            printf("%d ", i);
            for(int j = 0; j < nodes; j++){
                if(path_arr[i][j] != -1)
                    printf("%d ", path_arr[i][j]);
                else
                    break;
            }
            printf("\n");
        }
    }
}