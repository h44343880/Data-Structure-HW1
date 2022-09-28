#include<stdio.h>
#include<stdlib.h>

typedef struct Node{
    int mem_cnt, predecessor, length, overloaded;
    int * channel;// 紀錄與其他node之間channel數量的array
    char color;
}Node;

int min(int num1, int num2){
    return (num1 < num2) ? num1 : num2;
}

int main(){
    int nodes, links, requests;
    scanf("%d%d%d", &nodes, &links, &requests);// 讀入node數, link數和request數
    Node * node_arr = (Node*)malloc(sizeof(Node) * nodes);// 建立node的array
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

    int * req_start = (int*)malloc(sizeof(int) * requests);
    int * req_end = (int*)malloc(sizeof(int) * requests);
    for(int cnt = 0; cnt < requests; cnt++){// 讀入request
        scanf("%d%d%d", &id, &req_start[cnt], &req_end[cnt]);
    }

    int * queue = (int*)malloc(sizeof(int) * nodes);// 建立queue
    int front, back;// front為queue最前面元素的前一個位置的index, back為queue最後一個元素的index

    // int * loaded_arr = (int*)calloc(nodes, sizeof(int));
    // int * channel_sum_arr = (int*)calloc(nodes, sizeof(int));
    // for(int cnt = 0; cnt < requests; cnt++){
    //     loaded_arr[req_start[cnt]]++;
    //     loaded_arr[req_end[cnt]]++;
    // }
    // for(int cnt = 0; cnt < nodes; cnt++){
    //     for(int cnt2 = 0; cnt2 < nodes; cnt2++){
    //         channel_sum_arr[cnt] += node_arr[cnt].channel[cnt2];
    //     }
    // }
    // for(int i = 0; i < nodes; i++){
    //     printf("loaded: %d, channel_sum: %d\n", loaded_arr[i], channel_sum_arr[i]);
    // }

    int ** path_arr = (int**)calloc(requests, sizeof(int*));// 儲存accept request的路徑
    int * accept_arr = (int*)calloc(requests, sizeof(int));// 紀錄哪些request為accepted
    int accept_cnt = 0;// 紀錄accept request的數量    

    for(int cnt = 0; cnt < requests; cnt++){
        // 初始化
        front = -1;
        back = -1;
        for(int cnt2 = 0; cnt2 < nodes; cnt2++){
            node_arr[cnt2].color = 'w';
            node_arr[cnt2].predecessor = -1;
            node_arr[cnt2].length = 0;
            // node_arr[cnt2].overloaded = 0;
        }

        // 執行bfs找出shortest path
        queue[0] = req_start[cnt];// 將起點node推入queue中
        node_arr[queue[0]].color = 'g';
        back++;
        while(front != back){
            for(int cnt2 = 0; cnt2 < nodes; cnt2++){
                if(node_arr[queue[front + 1]].channel[cnt2]){
                    if(node_arr[cnt2].color == 'w'){
                        node_arr[cnt2].color = 'g';
                        node_arr[cnt2].length = node_arr[queue[front + 1]].length + 1;
                        node_arr[cnt2].predecessor = queue[front + 1];
                        // if(cnt2 != req_end[cnt])
                        //     node_arr[cnt2].overloaded = node_arr[front + 1].overloaded + min(0, min(node_arr[cnt2].mem_cnt, channel_sum_arr[cnt2]) - loaded_arr[cnt2] - 2);
                        // else
                        //     node_arr[cnt2].overloaded = node_arr[front + 1].overloaded + min(0, min(node_arr[cnt2].mem_cnt, channel_sum_arr[cnt2]) - loaded_arr[cnt2] - 1);
                        queue[++back] = cnt2;
                    }else if(node_arr[cnt2].color == 'g'){
                        // if(node_arr[node_arr[cnt2].predecessor].overloaded < node_arr[queue[front + 1]].overloaded)// 若下一個點的前一個點的overloaded比較小，代表負載比較大
                        //     node_arr[cnt2].predecessor = queue[front + 1];
                        // else if(node_arr[node_arr[cnt2].predecessor].overloaded == node_arr[queue[front + 1]].overloaded){
                        //     if(node_arr[queue[front + 1]].length < node_arr[node_arr[cnt2].predecessor].length)
                        //         node_arr[cnt2].predecessor = queue[front + 1];
                            // else 
                            if(node_arr[cnt2].length == node_arr[queue[front + 1]].length + 1){// 若到達下一個Node的路徑長一樣，則比較兩條路徑的資源多寡，取資源較多者的路徑
                                int min_resource1 = min(node_arr[node_arr[cnt2].predecessor].channel[cnt2], (node_arr[node_arr[cnt2].predecessor].mem_cnt));
                                int min_resource2 = min(node_arr[queue[front + 1]].channel[cnt2], node_arr[queue[front + 1]].mem_cnt);
                                if(min_resource1 < min_resource2)
                                    node_arr[cnt2].predecessor = queue[front + 1];
                            }
                        // }
                    }
                }
            }
            node_arr[queue[front + 1]].color = 'b';
            front++;
        }
        // loaded_arr[req_start[cnt]]--;
        // loaded_arr[req_end[cnt]]--;
        
        if(node_arr[req_end[cnt]].predecessor != -1){// 判斷是否有路徑通往終點，有的話才執行if內的程式碼
            int path_len = 1;
            int current = req_end[cnt];
            while(current != req_start[cnt]){// 計算path的長度
                path_len++;
                current = node_arr[current].predecessor;
            }
            int * path = (int*)malloc(sizeof(int) * nodes);
            for(int cnt2 = 0; cnt2 < nodes; cnt2++){
                path[cnt2] = -1;
            }

            // 檢查shortest path可不可行
            int accept = 1;
            current = node_arr[req_end[cnt]].predecessor;// 將current設為end的前一個node
            if(node_arr[req_end[cnt]].mem_cnt == 0 || node_arr[req_start[cnt]].mem_cnt == 0){ // 若起點或終點沒有memory，不可能accept
                accept = 0;
            }else{
                while(node_arr[current].predecessor != -1){
                    if(node_arr[current].mem_cnt < 2){
                        accept = 0;
                        break;
                    }
                    current = node_arr[current].predecessor;
                }
            }
            if(accept){
                accept_arr[cnt] = 1;// 表示該request accept
                accept_cnt++;
                current = req_end[cnt];
                // channel_sum_arr[current]--;
                path[path_len - 1] = current;
                while(current != req_start[cnt]){// 扣掉path用掉的資源
                    node_arr[current].mem_cnt--;
                    node_arr[node_arr[current].predecessor].mem_cnt--;
                    node_arr[current].channel[node_arr[current].predecessor]--;
                    node_arr[node_arr[current].predecessor].channel[current]--;
                    // 。。。。channel_sum_arr[node_arr[current].predecessor]--;
                    current = node_arr[current].predecessor;
                    path_len--;
                    path[path_len - 1] = current;
                }
                path_arr[cnt] = path;  
            }
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