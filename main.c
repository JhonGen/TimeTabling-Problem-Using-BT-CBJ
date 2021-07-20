#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#define SIZE_ID 3
#define MAX_EXAMS 20



struct student{
    char idStudent[SIZE_ID];
    int size;
    int *exams;
    struct student* next; 
};

typedef struct student student_t;

struct solution{
    int *solution;
    int score_min_ts;
    float score_spread;
};

typedef struct solution solution_t;

solution_t *bestSolution = NULL;

void printstudents(student_t *head){
    student_t *tmp = head;
    while(tmp!=NULL){
        printf("%s\n", tmp->idStudent);
        for(int i=0; i<tmp->size; i++){
            printf("%d",tmp->exams[i]);
        }
        printf("\n");
        tmp = tmp->next;
    }
}



student_t *find_student(student_t *head, char* id){
    student_t *tmp = head;
    while(tmp!=NULL){
        if(strcmp(tmp->idStudent, id)== 0){
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}

student_t *insert_student(student_t **head, student_t *student_to_insert){
    student_to_insert->next = *head;
    *head= student_to_insert;
    return student_to_insert;
}

int findSize(char file_name[]){
    FILE *fp = fopen(file_name, "r+");
    int c;
    int count=0;
    while(!feof(fp)){
        c = fgetc(fp);
        if(c == '\n'){
            count++;
        }
    }
    fclose(fp);
    return count;
}

void free_students(student_t *head){
    student_t *tmp = head;
    student_t *aux;
    while(tmp!=NULL){
        free(tmp->exams);
        aux = tmp;
        tmp = tmp->next;
        free(aux);
    }
}

int *create_conflict_matrix(student_t *head, int nE){
    int *conflict_matrix= (int*)calloc(nE*nE, sizeof(int)); 
    student_t *tmp = head;
    while(tmp!=NULL){
        for(int i =0; i<tmp->size; i++){
            for(int j= 0; j<tmp->size; j++){
                if(j!=i){
                  conflict_matrix[tmp->exams[i]*nE + tmp->exams[j]] += 1;
                }
            }
        }
        tmp = tmp->next;

    }
    return conflict_matrix;

}

bool feasibleSolution(solution_t *solution, int lvl, int *conflict_matrix, int nE){
    for(int i=0; i<lvl; i++){
        if((solution->solution[i] == solution->solution[lvl]) && conflict_matrix[i*nE + lvl]>0){
            return false;
        }
    }
    return true;
}

int score_solution_min_timeslots(solution_t *solution, int nT){
    int max = 0;
    for(int i=0; i<nT; i++){
        if(solution->solution[i] > max){
            max = solution->solution[i];
        }
    }
    return max;
}

int compare(const void* a, const void* b)
{
    return (*(int*)a - *(int*)b );
}

float score_solution_spread_timeslots(solution_t *solution,int nS, student_t *students){
    float score =0;
    student_t *tmp = students;
    int penalizaciones[5] = {16,8,4,2,1};
    while(tmp->next != NULL){
        int *exams_aux = (int*)malloc(sizeof(int)*tmp->size);
        for(int i=0; i<tmp->size;i++){
            exams_aux[i]= solution->solution[tmp->exams[i]];
        }
        qsort(exams_aux, tmp->size,sizeof(int),compare);
        for(int i=0; i<tmp->size-1;i++){
            if(exams_aux[i+1]-exams_aux[i] <5 ){
                score += penalizaciones[exams_aux[i+1]-exams_aux[i]];
            }
        }
        free(exams_aux);
        tmp = tmp->next;
    } 
    score /= nS;
    return score;

}

bool compare_solutions(solution_t *solution1, solution_t *solution2){
    if(solution1->score_min_ts < solution2->score_min_ts){
        return true;
    } else if(solution1->score_min_ts > solution2->score_min_ts){
        return false;
    } else{
        if(solution1->score_spread < solution2-> score_spread){
            return true;
        } else{
            return false;
        }
    }
}

void backtracking_cpu(solution_t *solution, int lvl, int nE, int nS, int nT, int *conflict_matrix, student_t *students){
    if(lvl == nE){
        if(feasibleSolution(solution,lvl-1,conflict_matrix,nE)){
            solution->score_min_ts = score_solution_min_timeslots(solution, nT);
            if(solution->score_min_ts <= bestSolution->score_min_ts){
                solution->score_spread = score_solution_spread_timeslots(solution, nS, students);
                if(compare_solutions(solution, bestSolution)){
                //memcpy(bestSolution->solution, solution->solution, sizeof(int)*nE);
                    for(int i=0; i<nE;i++){
                     bestSolution->solution[i]= solution->solution[i];
                    }
                    bestSolution->score_min_ts=solution->score_min_ts;
                    bestSolution->score_spread=solution->score_spread;
                }
            }
        }
        return;
    }
    if(feasibleSolution(solution, lvl-1, conflict_matrix, nE)){
        for(int i=0; i<nT; i++){
          solution->solution[lvl]= i; 
          backtracking_cpu(solution, lvl+1, nE, nS, nT, conflict_matrix, students);
        }
    }
}



int main(){
    FILE *exm, *stu;
    exm = fopen("YorkMills83.exm", "r+");
    stu = fopen("YorkMills83.stu", "r+");
    int nE = findSize("YorkMills83.exm");
    int nS = findSize("YorkMills83.stu");
    int nT = nE;
    //student_t *students=(student_t *)malloc(sizeof(student_t));
    student_t *students=NULL;
    char *idStudent= (char*)malloc(sizeof(char)*SIZE_ID);
    int *examid = (int*)malloc(sizeof(int));
    int line = fscanf(stu,"%s %d", idStudent, examid);
    while(line != EOF){
        student_t *temp_student = find_student(students, idStudent);        
        if(temp_student == NULL){
            student_t *new_student = (student_t*)malloc(sizeof(student_t));
            strcpy(new_student->idStudent,idStudent);
            new_student->size=0;
            new_student->exams = (int*)malloc(sizeof(int)*MAX_EXAMS);
            new_student->exams[new_student->size] = examid[0];
            new_student->size++;
            insert_student(&students, new_student);
        } else {
            temp_student->exams[temp_student->size] = examid[0];
            temp_student->size++;
        }
        line = fscanf(stu, "%s %d", idStudent, examid);
    }
    //printstudents(students);
    int *conflict_matrix = create_conflict_matrix(students, nE);

    /*for(int i=0; i<(nE*nE);i++){
        printf("%d ", conflict_matrix[i]);
        if((i+1)%nE==0){
            printf("\n");
        }
    }*/
    //solution_t *solution, int lvl, int nE, int nS,int nT, int *conflict_matrix, student_t *students
    bestSolution = (solution_t*)malloc(sizeof(solution_t));
    bestSolution->solution = (int *)malloc(sizeof(int)*nE);
    bestSolution->score_min_ts = nT+1;
    bestSolution->score_spread = 0;
    solution_t *initSolution=(solution_t*)malloc(sizeof(solution_t));
    initSolution->solution= (int *)malloc(sizeof(int)*nE);
    backtracking_cpu(initSolution, 0, nE, nS, nT, conflict_matrix, students);    
    for(int i=0; i<nE; i++){
        printf("%d ", bestSolution->solution[i]);
    } 
    printf("\ntimeslots: %d\n",bestSolution->score_min_ts);
    printf("score: %f\n",bestSolution->score_spread);

    free_students(students);
    free(idStudent);
    free(examid);
    free(conflict_matrix);
    free(initSolution);
    fclose(exm);
    fclose(stu);
    return 0;
}
