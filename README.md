# Deque 设计文档

## 1、数据结构实现： 
Deque 核心采用双向链表套循环数组的结构  
其中循环数组部分头部和尾部的插入为 $O(1)$ ， resize部分和线性表相同，均摊分析为 $O(1)$ , 随机插入时就近找头/尾进行移位，复杂度为最差（不引发扩容的情况下） $O(d/2)$ ,其中d为循环数组的长度，约等于 $\sqrt(n)$

## 2、Deque部分分裂和合并的思路：

### 1）、插入时
维护一个理想的块大小B = $\sqrt totalsize + 1$ （若B小于最小值8则取8） 
在进行所有的插入或者删除操作的时候，若当前块的大小大于B*2则分裂，若小于B/2则合并，注意若合并的时候如果合并后的块太大则不执行合并，而是两个中的数据匀一下各一半。

核心函数：
>1、块太大时分裂
```c++
typename double_list<CircularArr>::iterator split(typename double_list<CircularArr>::iterator block_it) {
            CircularArr &block = *block_it;
            int B = get_ideal_cap();

            if (block.size() <= 2 * B) return block_it;

            int keep_size = block.size() / 2;
            int move_size = block.size() - keep_size;

            CircularArr new_block(move_size + 4);
            //将原block的后一半移进新的block
            for (int i = keep_size; i < block.size(); ++i) {
                new_block.push_back(block[i]);
            }
            for (int i = 0; i < move_size; ++i) {
                block.pop_back();
            }

            auto new_block_it = list.insert_after(block_it, std::move(new_block));

            ++version;

            return new_block_it;
        }
```
>2、块太小时合并或匀一匀
```c++
typename double_list<CircularArr>::iterator maintain(typename double_list<CircularArr>::iterator block_it) {
            CircularArr &cur_block = *block_it;
            int B = get_ideal_cap();

            if (cur_block.size() >= B / 2 || list.size() <= 1) return block_it;

            auto node = block_it.ptr;
            auto right_node = node->next, left_node = node->prev;

            //先借右边
            if (right_node != list.tail) {
                CircularArr &right = right_node->data;
                int total = cur_block.size() + right.size();

                if (total <= 2 * B) {
                    for (int i = 0; i < right.size(); ++i) {
                        cur_block.push_back(right[i]);
                    }

                    typename double_list<CircularArr>::iterator right_it(right_node, &list);
                    list.erase(right_it);
                    ++version;
                    return block_it;
                } else {
                    int tar_size = total / 2;
                    int need_size = tar_size - cur_block.size();

                    for (int i = 0; i < need_size; ++i) {
                        cur_block.push_back(right[0]);
                        right.pop_front();
                    }
                    ++version;
                    return block_it;
                }
            } else {
                if (left_node != list.head) {
                    CircularArr &left = left_node->data;
                    int total = cur_block.size() + left.size();

                    if (total <= 2 * B) {
                        for (int i = left.size() - 1; i >= 0; --i) {
                            cur_block.push_front(left[i]);
                        }

                        typename double_list<CircularArr>::iterator left_it(left_node, &list);
                        list.erase(left_it);
                        ++version;
                        return block_it;
                    } else {
                        int tar_size = total / 2;
                        int need_size = tar_size - cur_block.size();

                        for (int i = 0; i < need_size; ++i) {
                            cur_block.push_front(left[left.size() - 1]);
                            left.pop_back();
                        }
                        ++version;
                        return block_it;
                    }
                }
            }
            //++version;
        }
```

### 2）、维护整个表防止复杂度退化
注意到若一个块太久没有被访问，其当时的理想大小可能已经远远偏离现在的理想大小，故需要进行维护  

实现方法为每次维护记录一个`base_line`,经历若干次插入和删除的操作后如果当前的理想大小`B`大于`4 * baseline`或者小于`baseline \ 4`则扫描所有的块并检查是否需要执行`split`或`maintain`
 
核心代码：
```c++
void check_and_rebuild() {
            int B = get_ideal_cap();
            if (B < base_line / 4 || B > base_line * 4) {
                for (auto it = list.begin(); it != list.end(); ++it) {
                    auto &block = *it;
                    if (block.size() == 0) {
                        if (list.size() > 1) {
                            it = list.erase(it);
                            --it;
                            continue;
                        }
                    }
                    if (block.size() > 2 * B) {
                        it = split(it);
                    } else if (block.size() < B / 2 && block.size() > 8) {
                        it = maintain(it);
                    }
                    //++it;
                }
                base_line = B;
            }
        }
```
 
### 3）、总体复杂度分析： 
在块大小理想的前提下，由循环数组的设计，随机插入的复杂度为$O(\sqrt n) + O(\sqrt n / 2)$，前者为找到对应循环数组的位置，后者为插入循环数组，总体为$O(\sqrt n)$  

split操作最差为进行 $\sqrt n$ 次随机插入操作后进行一次分裂，分裂操作复杂度最差为 $O(n)$ , 总体不会使复杂度退化 
 
maintain操作最差为进行 $\sqrt n$ 次随机删除操作后进行一次合并， 合并操作的复杂度不会大于 $O(n)$, 总体不会使复杂度退化 
 
## 3、迭代器失效的实现思路 
给迭代器设设置一个版本标识 `version` 所有会使迭代器失效的操作都会使这个标识+1，使用迭代器的时候先检查是否与创建是的 `version` 相同来判断是否失效 