#include<stdio.h>
#include<stdlib.h>
#include"my402list.h"
#include"cs402.h"


//initial
int My402ListInit(My402List* MyList){
    //My402ListElem* anchor = MyList->anchor.next;
    if(MyList == NULL)
        return FALSE;
    else
    {
        MyList->num_members=0;
        MyList->anchor.next = &(MyList->anchor);
        MyList->anchor.prev = &(MyList->anchor);
        return TRUE;
    }
}

//first
My402ListElem *My402ListFirst(My402List* MyList){
    //My402ListElem * anchor = &(MyList->anchor);
    //if(anchor->next != anchor)
    if(My402ListEmpty(MyList))
        return NULL;
    return MyList->anchor.next;
}

//last
My402ListElem *My402ListLast(My402List* MyList){
    //My402ListElem * anchor = &(MyList->anchor);
    //if(anchor->prev != anchor)
    if(My402ListEmpty(MyList))
        return NULL;
    return MyList->anchor.prev;
}

//next
My402ListElem *My402ListNext(My402List* MyList,My402ListElem* elem){
    //if(elem->next != &(MyList->anchor))
    if(elem == My402ListLast(MyList))
        return NULL;
    return elem->next;
}

//prev
My402ListElem *My402ListPrev(My402List* MyList, My402ListElem* elem){
    //if(elem->prev != &(MyList->anchor))
    if(elem == My402ListFirst(MyList))
        return NULL;
    return elem->prev;
}

//Find
My402ListElem *My402ListFind(My402List* MyList, void* obj){
    My402ListElem * temp = My402ListFirst(MyList);
    while(temp != NULL)
    {
        if(temp->obj == obj)
            return temp;
        else
        temp=My402ListNext(MyList,temp);
    }
    return NULL;
}
/*************length&empty******************/
//length
int My402ListLength(My402List* MyList){
    //My402ListElem * anchor = MyList->anchor;
    //int length;
    //My402ListElem * tmp = MyList->anchor.next;
    //length = 0;
    //while (tmp != &(MyList->anchor)){
    //    length++;
    //    tmp = tmp->next;
    //}
    return MyList->num_members;
}

//empty
int  My402ListEmpty(My402List* MyList){
    //int num=My402ListLength(MyList);
    //if(num==0)
    if(MyList->num_members == 0)
        return TRUE;
    return FALSE;
}
/**********************************************/

//Append
int  My402ListAppend(My402List* MyList, void* obj){
    My402ListElem * anchor = &(MyList->anchor);
    //MyList->Last = My402ListLast;
    //My402ListElem * lastone = MyList->Last(anchor);
    //My402ListElem * lastone = My402ListLast(MyList);

    //malloc new node
    My402ListElem * insert = (My402ListElem *)malloc(sizeof(My402ListElem));
    if(insert == NULL)
        return FALSE;
    else
    {
        //initial new one
        insert->obj = obj;
        insert->next = anchor;
        insert->prev = anchor->prev;
        //change old node
        anchor->prev->next = insert;
        anchor->prev = insert;
        //change the num
        MyList->num_members++;
        return TRUE;
     }
}

//prepend
int  My402ListPrepend(My402List* MyList, void* obj){
    My402ListElem * anchor =& (MyList -> anchor);
    //My402ListElem * firstone = My402ListFirst(MyList);

    //malloc new node
    My402ListElem * insert =(My402ListElem *)malloc(sizeof(My402ListElem));
    if(insert == NULL)
        return FALSE;
    else
    {
        //initial new one
        insert->obj = obj;
        insert->next = anchor->next;
        insert->prev = anchor;
        //change old node
        anchor->next->prev = insert;
        anchor->next = insert;
        //change the num
        MyList->num_members++;
        return TRUE;
    }
}

//after
int  My402ListInsertAfter(My402List* MyList, void* obj, My402ListElem* elem){
    if(elem == NULL)
    {
        //int ToF;
        return My402ListAppend(MyList,obj);
        //if(ToF == TRUE)
        //    return TRUE;
        //return FALSE;
    }
    else
    {
        //malloc new node
        My402ListElem * insert = (My402ListElem *)malloc(sizeof(My402ListElem));
        if(insert == NULL)
            return FALSE;
        else
        {
            //initial new node
            insert->prev = elem;
            insert->next = elem->next;
            insert->obj = obj;
            //change old node
            elem->next->prev = insert;
            elem->next = insert;
            //change the num
            MyList->num_members++;
            return TRUE;
        }
    }
}

//before
int  My402ListInsertBefore(My402List* MyList, void* obj, My402ListElem* elem){
    if(elem == NULL)
    {
        //int ToF;
        return My402ListPrepend(MyList, obj);
        //if(ToF == TRUE)
        //    return TRUE;
        //return FALSE;
    }
    else
    {
        //malloc new node
        My402ListElem * insert = (My402ListElem *)malloc(sizeof(My402ListElem));
        if(insert == NULL)
            return FALSE;
        else
        {
            //initial new node
            insert->prev = elem->prev;
            insert->next = elem;
            insert->obj = obj;
            //change old node
            elem->prev->next = insert;
            elem->prev = insert;
            //change the num
            MyList->num_members++;
            return TRUE;
        }
    }
}

/*********unlink*********************/
//Unlink
void My402ListUnlink(My402List* MyList, My402ListElem* elem){
    (elem->prev)->next = elem -> next;
    (elem->next)->prev = elem -> prev;
    free(elem);
    //change the num
    MyList->num_members--;
}

//Unlinkall
void My402ListUnlinkAll(My402List* MyList){
    //My402ListElem * anchor = &(MyList->anchor);
    //initial
    My402ListElem * temp = MyList->anchor.next;
    My402ListElem * tempnext = temp->next;
    while(temp != &(MyList->anchor)){
        My402ListUnlink(MyList, temp);
        temp=tempnext;
        tempnext=temp->next;
    }
    //delete all
    //anchor->next = anchor;
    //anchor->prev = anchor;
}
/************************************/
