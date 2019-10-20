#include "Factory.h"
#include <assert.h>

int startSimpleBuyerTest(){
    Product p1(1,1);
    Product p2(2,2);
    Product p3(3,3);
    Product p4(4,4);
    Product p5(5,5);
    Product p6(6,6);
    Product productArr[2];
    Product productArr1[3];
    Product productArr2[1];
    productArr[0]=p1;
    productArr[1]=p2;
    productArr1[0]=p3;
    productArr1[1]=p4;
    productArr1[2]=p5;
    productArr2[0]=p6;
    Factory factory;
    factory.openFactory();
    factory.startSimpleBuyer(3);
    assert(factory.finishSimpleBuyer(3) == -1);
    factory.startProduction(2,productArr,1);
    factory.finishProduction(1);
    factory.startSimpleBuyer(4);
    assert(factory.finishSimpleBuyer(4) == 1);
    factory.closeFactory();
    return 0;
}

int produceTest(){
    Product p1(1,1);
    Product p2(2,2);
    Product p3(3,3);
    Product p4(4,4);
    Product p5(5,5);
    Product p6(6,6);
    Product productArr[2];
    Product productArr1[3];
    Product productArr2[1];
    productArr[0]=p1;
    productArr[1]=p2;
    productArr1[0]=p3;
    productArr1[1]=p4;
    productArr1[2]=p5;
    productArr2[0]=p6;
    Factory factory;
    factory.openFactory();
    factory.startProduction(2,productArr,1);
    factory.finishProduction(1);
    factory.startSimpleBuyer(4);
    assert(factory.finishSimpleBuyer(4) == 1);
    factory.produce(3,productArr1);
    assert(factory.tryBuyOne() == 2);
    factory.closeFactory();
    return 0;
}

int startProductionTest(){
    Product p1(1,1);
    Product p2(2,2);
    Product p3(3,3);
    Product p4(4,4);
    Product p5(5,5);
    Product p6(6,6);
    Product productArr[2];
    Product productArr1[3];
    Product productArr2[1];
    productArr[0]=p1;
    productArr[1]=p2;
    productArr1[0]=p3;
    productArr1[1]=p4;
    productArr1[2]=p5;
    productArr2[0]=p6;
    Factory factory;
    factory.openFactory();
    factory.startProduction(2,productArr,1);
    factory.finishProduction(1);
    factory.startSimpleBuyer(4);
    assert(factory.finishSimpleBuyer(4) == 1);
    factory.closeFactory();
    return 0;
}

int startCompanyBuyerTest(){
    Product p1(1,1);
    Product p2(2,2);
    Product p3(3,3);
    Product p4(4,4);
    Product p5(5,5);
    Product p6(6,6);
    Product productArr[2];
    Product productArr1[3];
    Product productArr2[1];
    productArr[0]=p1;
    productArr[1]=p2;
    productArr1[0]=p3;
    productArr1[1]=p4;
    productArr1[2]=p5;
    productArr2[0]=p6;
    Factory factory;
    factory.openFactory();
    factory.startProduction(2,productArr,1);
    factory.finishProduction(1);
    factory.startSimpleBuyer(4);
    assert(factory.finishSimpleBuyer(4) == 1);
    factory.produce(3,productArr1);
    assert(factory.tryBuyOne() == 2);
    factory.startCompanyBuyer(2, 1, 20);
    assert(factory.finishCompanyBuyer(20) == 0);
    factory.startCompanyBuyer(1, 7, 21);
    assert(factory.finishCompanyBuyer(21) == 1);
    factory.startCompanyBuyer(2, 6, 22);
    factory.startProduction(1,productArr2,30);
    factory.finishProduction(30);
    assert(factory.finishCompanyBuyer(22) == 1);
    factory.closeFactory();
    return 0;
}

int startThiefTest(){
    Product p1(1,1);
    Product p2(2,2);
    Product p3(3,3);
    Product p4(4,4);
    Product p5(5,5);
    Product p6(6,6);
    Product productArr[2];
    Product productArr1[3];
    Product productArr2[1];
    productArr[0]=p1;
    productArr[1]=p2;
    productArr1[0]=p3;
    productArr1[1]=p4;
    productArr1[2]=p5;
    productArr2[0]=p6;
    Factory factory;
    factory.openFactory();
    factory.startThief(1,20);
    assert(factory.finishThief(20) == 0);
    factory.startProduction(2,productArr,1);
    factory.finishProduction(1);
    factory.startThief(1,21);
    assert(factory.finishThief(21) == 1);
    //1 items left
    factory.produce(3,productArr1);
    //4 items left
    factory.startThief(1,22);
    factory.startThief(1,23);
    assert(factory.finishThief(22) == 1);
    assert(factory.finishThief(23) == 1);
    //2 items left
    factory.startThief(3,24);
    factory.startThief(1,25);
    int res1 = factory.finishThief(24);
    int res2 = factory.finishThief(25);
    assert((res1 == 2 && res2 == 0) || (res2 == 1 && res1 == 1));
    factory.closeFactory();
    return 0;
}

int startOpenCloseTest(){
    Product p1(1,1);
    Product p2(2,2);
    Product p3(3,3);
    Product p4(4,4);
    Product p5(5,5);
    Product p6(6,6);
    Product productArr[2];
    Product productArr1[3];
    Product productArr2[1];
    productArr[0]=p1;
    productArr[1]=p2;
    productArr1[0]=p3;
    productArr1[1]=p4;
    productArr1[2]=p5;
    productArr2[0]=p6;
    Factory factory;
    factory.startProduction(2,productArr,1);
    factory.finishProduction(1);
    factory.produce(3,productArr1);
    factory.openFactory();
    factory.startSimpleBuyer(2);
    factory.finishSimpleBuyer(2);
    factory.startThief(1,20);
    assert(factory.finishThief(20) == 1);
    factory.closeFactory();
    factory.startSimpleBuyer(3);
    assert(factory.finishSimpleBuyer(3) == -1);
    factory.startCompanyBuyer(2,1,19);
    factory.openFactory();
    assert(factory.finishCompanyBuyer(19) == 0);
    factory.closeFactory();
    return 0;
}

int startOpenCloseRetServiceTest(){
    Product p1(1,1);
    Product p2(2,2);
    Product p3(3,3);
    Product p4(4,4);
    Product p5(5,5);
    Product p6(6,6);
    Product productArr[2];
    Product productArr1[3];
    Product productArr2[1];
    productArr[0]=p1;
    productArr[1]=p2;
    productArr1[0]=p3;
    productArr1[1]=p4;
    productArr1[2]=p5;
    productArr2[0]=p6;
    Factory factory;
    factory.closeReturningService();
    factory.startProduction(2,productArr,1);
    factory.finishProduction(1);
    factory.produce(3,productArr1);
    factory.startSimpleBuyer(2);
    factory.finishSimpleBuyer(2);
    factory.startThief(1,20);
    assert(factory.finishThief(20) == 1);
    factory.startCompanyBuyer(2,1,19);
    assert(factory.finishCompanyBuyer(19) == 0);
    factory.startCompanyBuyer(1,19,29);
    factory.openReturningService();
    assert(factory.finishCompanyBuyer(29) == 1);
    factory.closeFactory();
    return 0;
}

int listStolenProductsTest(){
    Product p1(1,1);
    Product p2(2,2);
    Product p3(3,3);
    Product p4(4,4);
    Product p5(5,5);
    Product p6(6,6);
    Product productArr[2];
    Product productArr1[3];
    Product productArr2[1];
    productArr[0]=p1;
    productArr[1]=p2;
    productArr1[0]=p3;
    productArr1[1]=p4;
    productArr1[2]=p5;
    productArr2[0]=p6;
    Factory factory;
    factory.startProduction(2,productArr,1);
    factory.finishProduction(1);
    factory.produce(3,productArr1);
    //5 items left
    factory.startThief(2,20);
    factory.startThief(2,21);
    assert(factory.finishThief(21) == 2);
    assert(factory.finishThief(20) == 2);
    factory.startThief(1,23);
    assert(factory.finishThief(23) == 1);
    std::list<std::pair<Product, int> > stoList;
    stoList = factory.listStolenProducts();
    for(int i=0 ; i<5 ; i++){
        Product p  = (*stoList.begin()).first;
        assert(p.getId() == (i+1));
        stoList.erase(stoList.begin());
    }
    factory.closeFactory();
    return 0;
}

int listAvailableProductsTest(){
    Product p1(1,1);
    Product p2(2,2);
    Product p3(3,3);
    Product p4(4,4);
    Product p5(5,5);
    Product p6(6,6);
    Product productArr[2];
    Product productArr1[3];
    Product productArr2[1];
    productArr[0]=p1;
    productArr[1]=p2;
    productArr1[0]=p3;
    productArr1[1]=p4;
    productArr1[2]=p5;
    productArr2[0]=p6;
    Factory factory;
    factory.startProduction(2,productArr,1);
    factory.finishProduction(1);
    factory.produce(3,productArr1);
    //5 items left
    std::list<Product> avlList;
    avlList = factory.listAvailableProducts();
    for(int i=0 ; i<5 ; i++){
        Product p  = *avlList.begin();
        assert(p.getId() == (i+1));
        avlList.erase(avlList.begin());
    }
    factory.closeFactory();
    return 0;
}

int main(){
    startSimpleBuyerTest();
    produceTest();
    startProductionTest();
    startCompanyBuyerTest();
    startThiefTest();
    startOpenCloseTest();
    startOpenCloseRetServiceTest();
    listStolenProductsTest();
    listAvailableProductsTest();
    return 0;
}
