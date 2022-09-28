#include<stdio.h>
#include<stdlib.h>

typedef struct Node{
    int mem_cnt, predecessor, length, overloaded;
    int * channel;// 紀錄與其他node之間channel數量的array
    char color;
}Node;

Node * node_arr;
int nodes, links, requests, front, back, accept_cnt;// front為queue最前面元素的前一個位置的index, back為queue最後一個元素的index
int * req_start, * req_end, * queue, * accept_arr;
int ** path_arr;
int * loaded_arr, * channel_sum_arr;

int min(int num1, int num2){
    return (num1 < num2) ? num1 : num2;
}

int overload_cnt(int node_id){
    return min(0, min(node_arr[node_id].mem_cnt, channel_sum_arr[node_id]) - loaded_arr[node_id] - 2);
}

void BFS(Node * arr, int start, int end){
    queue[0] = start;
    node_arr[queue[0]].color = 'g';
    back++;
    while(front != back){
        for(int cnt = 0; cnt < nodes; cnt++){
            if(node_arr[queue[front + 1]].channel[cnt]){
                if(node_arr[cnt].color == 'w'){
                    node_arr[cnt].color = 'g';
                    node_arr[cnt].predecessor = queue[front + 1];
                    node_arr[cnt].length = node_arr[queue[front + 1]].length + 1;
                    node_arr[cnt].overloaded += node_arr[queue[front + 1]].overloaded;
                    queue[++back] = cnt;
                }else if(node_arr[cnt].color == 'g'){
                    if(node_arr[node_arr[cnt].predecessor].overloaded < node_arr[queue[front + 1]].overloaded){// 若下一個點的前一個點的overloaded比較小，代表負載比較大
                        node_arr[cnt].predecessor = queue[front + 1];
                        if(cnt != end)
                            node_arr[cnt].overloaded += node_arr[node_arr[cnt].predecessor].overloaded;
                    }else if(node_arr[node_arr[cnt].predecessor].overloaded == node_arr[queue[front + 1]].overloaded){
                        if(node_arr[queue[front + 1]].length < node_arr[node_arr[cnt].predecessor].length){
                            node_arr[cnt].predecessor = queue[front + 1];
                            if(cnt != end)
                                node_arr[cnt].overloaded += node_arr[node_arr[cnt].predecessor].overloaded;
                        }else if(node_arr[cnt].length == node_arr[queue[front + 1]].length + 1){// 若到達下一個Node的路徑長一樣，則比較兩條路徑的資源多寡，取資源較多者的路徑
                            int min_resource1 = min(node_arr[node_arr[cnt].predecessor].channel[cnt], (node_arr[node_arr[cnt].predecessor].mem_cnt));
                            int min_resource2 = min(node_arr[queue[front + 1]].channel[cnt], node_arr[queue[front + 1]].mem_cnt);
                            if(min_resource1 < min_resource2)
                                node_arr[cnt].predecessor = queue[front + 1];
                        }
                    }
                }
            }
        }
        node_arr[queue[front + 1]].color = 'b';
        front++;
    }
}

int verify(int start, int end){
    if(node_arr[end].predecessor != -1){// 判斷是否有路徑通往終點
        // 檢查shortest path可不可行
        int path_len = 2;
        int current = node_arr[end].predecessor;// 將current設為end的前一個node
        if(node_arr[end].mem_cnt == 0 || node_arr[start].mem_cnt == 0){ // 若起點或終點沒有memory，不可能accept
            return -1;
        }else{
            while(node_arr[current].predecessor != -1){
                if(node_arr[current].mem_cnt < 2){
                    return -1;
                }
                path_len++;
                current = node_arr[current].predecessor;
            }
        }
        return path_len;
    }else
        return -1;
}

void store(int req_id, int len){
    int * path = (int*)malloc(sizeof(int) * nodes);
    for(int cnt = 0; cnt < nodes; cnt++){
        path[cnt] = -1;
    }
    accept_arr[req_id] = 1;// 表示該request accept
    accept_cnt++;
    int current = req_end[req_id];
    // channel_sum_arr[current]--;
    path[len - 1] = current;
    while(current != req_start[req_id]){// 扣掉path用掉的資源
        node_arr[current].mem_cnt--;
        node_arr[node_arr[current].predecessor].mem_cnt--;
        node_arr[current].channel[node_arr[current].predecessor]--;
        node_arr[node_arr[current].predecessor].channel[current]--;
        // channel_sum_arr[node_arr[current].predecessor]--;
        current = node_arr[current].predecessor;
        len--;
        path[len - 1] = current;
    }
    path_arr[req_id] = path;  
}

int main(){
    
    scanf("%d%d%d", &nodes, &links, &requests);// 讀入node數, link數和request數
    node_arr = (Node*)malloc(sizeof(Node) * nodes);// 建立node的array
    int id, mem_cnt;
    for(int cnt = 0; cnt < nodes; cnt++){// 讀入每個node的memory數
        scanf("%d%d", &id, &mem_cnt);
        node_arr[id].mem_cnt = mem_cnt;
        node_arr[id].channel = (int*)calloc(nodes, sizeof(int));// 為每個node建立channel的array
    }
    int first, second, channel_num;
    for(int cnt = 0; cnt < links; cnt++){// 讀入node之間的channel數
        scanf("%d%d%d%d", &id, &first, &second, &channel_num);
        node_arr[first].channel[second] = channel_num;
        node_arr[second].channel[first] = channel_num; 
    }

    req_start = (int*)malloc(sizeof(int) * requests);
    req_end = (int*)malloc(sizeof(int) * requests);
    for(int cnt = 0; cnt < requests; cnt++){// 讀入request
        scanf("%d%d%d", &id, &req_start[cnt], &req_end[cnt]);
    }
    queue = (int*)malloc(sizeof(int) * nodes);// 建立queue

    loaded_arr = (int*)calloc(nodes, sizeof(int));
    channel_sum_arr = (int*)calloc(nodes, sizeof(int));
    for(int cnt = 0; cnt < requests; cnt++){// 計算最少需要多少資源才可以滿足request
        loaded_arr[req_start[cnt]]++;
        loaded_arr[req_end[cnt]]++;
    }
    for(int cnt = 0; cnt < nodes; cnt++){// 計算node總共有多少channel
        for(int cnt2 = 0; cnt2 < nodes; cnt2++){
            channel_sum_arr[cnt] += node_arr[cnt].channel[cnt2];
        }
    }

    path_arr = (int**)calloc(requests, sizeof(int*));// 儲存accept request的路徑
    accept_arr = (int*)calloc(requests, sizeof(int));// 紀錄哪些request為accepted
    accept_cnt = 0;// 紀錄accept request的數量

    for(int cnt = 0; cnt < requests; cnt++){
        // 初始化
        front = -1;
        back = -1;
        for(int cnt2 = 0; cnt2 < nodes; cnt2++){
            node_arr[cnt2].color = 'w';
            node_arr[cnt2].predecessor = -1;
            node_arr[cnt2].length = 0;
            node_arr[cnt2].overloaded = overload_cnt(cnt2);
        }

        // 執行bfs找出shortest path
        BFS(node_arr, req_start[cnt], req_end[cnt]);

        loaded_arr[req_start[cnt]]--;
        loaded_arr[req_end[cnt]]--;

        int path_len = verify(req_start[cnt], req_end[cnt]);// 驗證path是否可行
        if(path_len != -1){
            store(cnt, path_len);
        }
    }
    // 輸出結果
    printf("%d\n", accept_cnt);
    for(int cnt = 0; cnt < requests; cnt++){
        if(accept_arr[cnt]){
            printf("%d ", cnt);
            for(int cnt2 = 0; cnt2 < nodes; cnt2++){
                if(path_arr[cnt][cnt2] != -1)
                    printf("%d ", path_arr[cnt][cnt2]);
                else
                    break;
            }
            printf("\n");
        }
    }
}