#include<stdio.h>
#include<stdlib.h>

typedef struct Node{
    int mem_cnt;// 此node的memory數量
    int * channel;// 紀錄與其他node之間channel數量的array
}Node;

int main(){
    int nodes, links, requests;
    scanf("%d%d%d", &nodes, &links, &requests);// 讀入node數, link數和request數
    Node * node_arr = (Node*)malloc(sizeof(Node) * nodes);// 建立node的array
    int id, mem_cnt;
    for(int i = 0; i < nodes; i++){// 讀入每個node的memory數
        scanf("%d%d", &id, &mem_cnt);
        node_arr[id].mem_cnt = mem_cnt;
        node_arr[id].channel = (int*)calloc(nodes,sizeof(int));// 為每個node建立channel的array
    }
    int first, second, cnt;
    for(int i = 0; i < links; i++){// 讀入node之間的channel數
        scanf("%d%d%d%d", &id, &first, &second, &cnt);
        node_arr[first].channel[second] = cnt;
        node_arr[second].channel[first] = cnt; 
    }

    int * req_start = (int*)malloc(sizeof(int) * requests);
    int * req_end = (int*)malloc(sizeof(int) * requests);
    for(int i = 0; i < requests; i++){// 讀入request
        scanf("%d%d%d", &id, &req_start[i], &req_end[i]);
    }

    int * queue = (int*)malloc(sizeof(int) * nodes);// 建立queue
    int front, back;// front為queue最前面元素的前一個位置的index, back為queue最後一個元素的index

    char * color_arr = (char*)malloc(sizeof(char) * nodes);// 紀錄node的顏色
    int * predecessor_arr = (int*)malloc(sizeof(int) * nodes);// 紀錄node被哪個node找到
    int * length_arr = (int*)malloc(sizeof(int) * nodes);// 紀錄起點到node的路徑長

    int ** path_arr = (int**)calloc(requests, sizeof(int*));// 儲存accept request的路徑
    int * accept_arr = (int*)calloc(requests, sizeof(int));// 紀錄哪些request為accepted
    int accept_cnt = 0;// 紀錄accept request的數量    

    for(int i = 0; i < requests; i++){
        // 初始化
        front = -1;
        back = -1;
        for(int j = 0; j < nodes; j++){
            color_arr[j] = 'w';
            predecessor_arr[j] = -1;
            length_arr[j] = 0;
        }

        // 執行bfs找出shortest path
        queue[0] = req_start[i];// 將起點node推入queue中
        color_arr[queue[0]] = 'g';
        back++;
        while(front != back){
            for(int j = 0; j < nodes; j++){
                if(node_arr[queue[front + 1]].channel[j]){
                    if(color_arr[j] == 'w'){
                        color_arr[j] = 'g';
                        length_arr[j] = length_arr[queue[front + 1]] + 1;
                        predecessor_arr[j] = queue[front + 1];
                        queue[++back] = j;
                    }else if(color_arr[j] == 'g'){
                        if(length_arr[j] == length_arr[queue[front + 1]] + 1){// 若到達下一個Node的路徑長一樣，則比較兩條路徑的資源多寡，取資源較多者的路徑
                            int min_resource1 = (node_arr[predecessor_arr[j]].channel[j] < (node_arr[predecessor_arr[j]].mem_cnt)) ? node_arr[predecessor_arr[j]].channel[j] : (node_arr[predecessor_arr[j]].mem_cnt);
                            int min_resource2 = (node_arr[queue[front + 1]].channel[j] < node_arr[queue[front + 1]].mem_cnt) ? node_arr[queue[front + 1]].channel[j] : node_arr[queue[front + 1]].mem_cnt;
                            if(min_resource1 < min_resource2)
                            predecessor_arr[j] = queue[front + 1];
                        }
                    }
                }
            }
            color_arr[queue[front + 1]] = 'b';
            front++;
        }
        if(predecessor_arr[req_end[i]] != -1){// 判斷是否有路徑通往終點，有的話才執行if內的程式碼
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
            }else{
                while(predecessor_arr[current] != -1){
                    if(node_arr[current].mem_cnt < 2){
                        accept = 0;
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