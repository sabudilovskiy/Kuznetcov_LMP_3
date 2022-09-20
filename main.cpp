#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <thread>
#include <utility>
struct Answer{
    int max;
    size_t count = 0;
};


void producer(std::vector<std::vector<int>>& matrix, size_t begin, size_t end, Answer& local){
    Answer answer;
    for (size_t i = begin; i <= end; ++i){
        auto max_i = *std::max_element(matrix[i].begin(), matrix[i].end());
        if (answer.count == 0 || answer.max < max_i){
            answer.count = 1;
            answer.max = max_i;
        }
        else if (answer.max == max_i){
            ++answer.count;
        }
    }
    local = answer;
}


size_t task(std::vector<std::vector<int>>& matrix, size_t thread_size) {
    if (matrix.size() < thread_size) thread_size = matrix.size();
    std::vector<std::thread> threads;
    std::vector<Answer> local_answers;
    threads.reserve(thread_size);
    local_answers.resize(thread_size);
    size_t count_one_thread = matrix.size() / thread_size;
    size_t cur_begin = 0;
    size_t cur_end = count_one_thread - 1;
    for (size_t i = 0; i < thread_size - 1; ++i){
        threads.emplace_back(&producer, std::ref(matrix), cur_begin, cur_end, std::ref(local_answers[i]));
        cur_begin+=count_one_thread;
        cur_end+=count_one_thread;
    }
    threads.emplace_back(&producer, std::ref(matrix), cur_begin, matrix.size() - 1, std::ref(local_answers[thread_size - 1]));
    for (auto& thread : threads){
        thread.join();
    }
    Answer global_answer = local_answers[0];
    for (size_t i = 1; i < thread_size; ++i){
        if (global_answer.max < local_answers[i].max){
            global_answer = local_answers[i];
        }
        else if (global_answer.max == local_answers[i].max){
            ++global_answer.count;
        }
    }
    return global_answer.count;
}

int main() {
    std::vector<std::vector<int>> matrix;
    size_t n, k;
    std::ifstream file_in("in.txt");
    file_in >> n >> k;
    matrix.resize(n);
    for (size_t i = 0; i < n; ++i){
        matrix[i].resize(k);
        for (size_t j = 0; j < k; ++j){
            file_in >> matrix[i][j];
        }
    }
    std::cout << task(matrix, 8);
}
