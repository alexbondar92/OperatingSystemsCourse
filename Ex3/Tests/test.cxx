#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include "Factory.h"
#include "test_utilities.h"

#define TEST_SYNC_SIZE 100 // this is not recommended to increase this parameter.
#define STRESS_TEST_SIZE 10000 // with this parameter you can do whatever you want

using namespace std;





bool testProduce(){
	Product products[4];
	products[0]=Product(1,3);
	products[1]=Product(2,5);
	products[2]=Product(3,7);
	products[3]=Product(4,9);
	
	Factory factory_1=Factory();
	for(int i=0; i<4; i++){
		factory_1.produce(1, products+i);
	}
	int temp_ids[4] = {1,2,3,4};
	int temp_values[4] = {3,5,7,9};
	int i=0;	
	list<Product> avProds=factory_1.listAvailableProducts();
	ASSERT_TEST(avProds.size() == 4);
	for (list<Product>::iterator iterator = avProds.begin(), end = avProds.end(); iterator != end; ++iterator) {
		ASSERT_TEST((*iterator).getId() == temp_ids[i] && (*iterator).getValue() == temp_values[i]);
		i++;
	}
	
	Factory factory_2=Factory();
	factory_2.produce(3, products);
	int temp_ids2[3] = {1,2,3};
	int temp_values2[3] = {3,5,7};
	i=0;	
	avProds=factory_2.listAvailableProducts();
	ASSERT_TEST(avProds.size() == 3);
	for (list<Product>::iterator iterator = avProds.begin(), end = avProds.end(); iterator != end; ++iterator) {
		ASSERT_TEST((*iterator).getId() == temp_ids2[i] && (*iterator).getValue() == temp_values2[i]);
		i++;
	}
	
	return true;
}


bool testTryBuyOne(){
	Product products[4];
	products[0]=Product(1,3);
	products[1]=Product(2,5);
	products[2]=Product(3,7);
	products[3]=Product(4,9);
	
	Factory factory=Factory();
	
	ASSERT_TEST(factory.tryBuyOne() == -1);
	
	factory.produce(4, products);
	
	ASSERT_TEST(factory.tryBuyOne() == 1);
	ASSERT_TEST(factory.tryBuyOne() == 2);
	ASSERT_TEST(factory.tryBuyOne() == 3);
	ASSERT_TEST(factory.tryBuyOne() == 4);
	
	ASSERT_TEST(factory.tryBuyOne() == -1);
	
	return true;
}


bool testBuyProducts(){
	Product products[4];
	products[0]=Product(1,3);
	products[1]=Product(2,5);
	products[2]=Product(3,7);
	products[3]=Product(4,9);
	
	Factory factory=Factory();
	
	factory.produce(4, products);
	
	list<Product> bought_products = factory.buyProducts(2);
	list<Product>::iterator iterator = bought_products.begin();
	ASSERT_TEST((*iterator).getId() == 1 && (*iterator).getValue() == 3);
	iterator++;
	ASSERT_TEST((*iterator).getId() == 2 && (*iterator).getValue() == 5);
	
	bought_products = factory.buyProducts(1);
	iterator = bought_products.begin();
	ASSERT_TEST((*iterator).getId() == 3 && (*iterator).getValue() == 7);
	
	bought_products = factory.buyProducts(1);
	iterator = bought_products.begin();
	ASSERT_TEST((*iterator).getId() == 4 && (*iterator).getValue() == 9);
	
	return true;
}


bool testReturnProducts(){
	Factory factory=Factory();

	list<Product> avProds=factory.listAvailableProducts();
	ASSERT_TEST(avProds.size() == 0);
	
	list<Product> products_to_return;
	Product products[6];
	products[0]=Product(1,3);
	products[1]=Product(2,5);
	products[2]=Product(3,7);
	products[3]=Product(4,9);
	products[4]=Product(5,11);
	products[5]=Product(6,13);
	products_to_return.push_back(products[0]);
	products_to_return.push_back(products[1]);
	factory.returnProducts(products_to_return,0);
	
	avProds=factory.listAvailableProducts();
	ASSERT_TEST(avProds.size() == 2);

	factory.produce(2, products+2);
	
	avProds=factory.listAvailableProducts();
	ASSERT_TEST(avProds.size() == 4);
	
	list<Product> products_to_return2;
	products_to_return2.push_back(products[4]);
	products_to_return2.push_back(products[5]);
	factory.returnProducts(products_to_return2,0);
	
	avProds=factory.listAvailableProducts();
	ASSERT_TEST(avProds.size() == 6);
	int temp_ids[6] = {1,2,3,4,5,6};
	int temp_values[6] = {3,5,7,9,11,13};
	int i=0;
	for (list<Product>::iterator iterator = avProds.begin(), end = avProds.end(); iterator != end; ++iterator) {
		ASSERT_TEST((*iterator).getId() == temp_ids[i] && (*iterator).getValue() == temp_values[i]);
		i++;
	}
	
	return true;
}


bool testStandardRun() {
	Factory factory=Factory();
	int result;
	Product products[3];
	products[0]=Product(1,3);
	products[1]=Product(2,5);
	products[2]=Product(3,7);
	
	factory.startProduction(3, products, 1);
	factory.finishProduction(1);
	
	factory.startCompanyBuyer(2, 4, 2);
	ASSERT_TEST(factory.finishCompanyBuyer(2) == 1);
	
	factory.startSimpleBuyer(3);
	ASSERT_TEST(factory.finishSimpleBuyer(3) == 3);
	
	list<Product> avProds;
	avProds=factory.listAvailableProducts();
	ASSERT_TEST(avProds.size() == 1);
	ASSERT_TEST((*(avProds.begin())).getId() == 1 && (*(avProds.begin())).getValue() == 3);
	
	factory.startThief(17, 842);
	ASSERT_TEST(factory.finishThief(842) == 1);
	
	list<pair<Product, int> > stolProds=factory.listStolenProducts();
	ASSERT_TEST(stolProds.size() == 1);
	ASSERT_TEST(((*(stolProds.begin())).first).getId() == 1 && ((*(stolProds.begin())).first).getValue() == 3 && (*(stolProds.begin())).second == 842);
	return true;
}

bool testOpenAndClose() {
	Factory factory=Factory();
	factory.openFactory();
	factory.closeFactory();
	factory.closeFactory();
	
	Product products_2[3];
	products_2[0]=Product(1,3);
	products_2[1]=Product(2,5);
	products_2[2]=Product(3,7);
	Product products_1[4];
	products_1[0]=Product(4,11);
	products_1[1]=Product(5,17);
	products_1[2]=Product(6,2);
	products_1[3]=Product(7,1);
	
	factory.startCompanyBuyer(7, 4, 11); // company that should run 3rd after opening, only when more products will be added
	
	factory.startProduction(4, products_1, 1);
	factory.finishProduction(1);
	
	factory.startSimpleBuyer(1001);
	ASSERT_TEST(factory.finishSimpleBuyer(1001) == -1);
	
	factory.startProduction(3, products_2, 2);
	factory.finishProduction(2);
	
	factory.startCompanyBuyer(1, 1, 12); // company that should run 2nd after opening
	
	factory.startThief(2, 101); // thief should run 1st
	
	int temp_ids[7] = {4,5,6,7,1,2,3};
	int temp_values[7] = {11,17,2,1,3,5,7};
	int i=0;	
	list<Product> avProds=factory.listAvailableProducts();
	ASSERT_TEST(avProds.size() == 7);
	for (list<Product>::iterator iterator = avProds.begin(), end = avProds.end(); iterator != end; ++iterator) {
		ASSERT_TEST((*iterator).getId() == temp_ids[i] && (*iterator).getValue() == temp_values[i]);
		i++;
	}
	
	factory.openFactory(); // now the massacre begins
	
    int tmp = factory.finishThief(101);
//    std::cout << "line 226: first = " << tmp << std::endl;
    
	ASSERT_TEST(tmp == 2); // thief run first
	
	list<pair<Product, int> > stolProds=factory.listStolenProducts();
	ASSERT_TEST(stolProds.size() == 2);
	
	ASSERT_TEST(factory.finishCompanyBuyer(12) == 0); // company 11 can't run, so company 12 should run
	
	factory.startThief(1, 102);
	ASSERT_TEST(factory.finishThief(102) == 1); // a thief will take the product, after company 12 that has already finished
	
	stolProds=factory.listStolenProducts();
	ASSERT_TEST(stolProds.size() == 3);
	list<pair<Product, int> >::iterator iterator = stolProds.begin();
    
//    std::cout << "line 239: first = " << ((*(iterator)).first).getId() << " second = " << ((*(iterator)).first).getValue() << " third = " << (*(iterator)).second << std::endl;
    
	ASSERT_TEST(((*(iterator)).first).getId() == 4 && ((*(iterator)).first).getValue() == 11 && (*(iterator)).second == 101);
	++iterator;
	ASSERT_TEST(((*(iterator)).first).getId() == 5 && ((*(iterator)).first).getValue() == 17 && (*(iterator)).second == 101);
	++iterator;
	ASSERT_TEST(((*(iterator)).first).getId() == 7 && ((*(iterator)).first).getValue() == 1 && (*(iterator)).second == 102);
	
	Product products_3[5];
	products_3[0]=Product(10,8);
	products_3[1]=Product(11,11);
	products_3[2]=Product(12,14);
	products_3[3]=Product(100,12);
	products_3[4]=Product(15,170);
	factory.startProduction(5, products_3, 3);
	factory.finishProduction(3);
	
	ASSERT_TEST(factory.finishCompanyBuyer(11) == 1);
	
	int temp_ids2[2] = {15,1};
	int temp_values2[2] = {170,3};
	i=0;
	avProds=factory.listAvailableProducts();
	ASSERT_TEST(avProds.size() == 2);
	for (list<Product>::iterator iterator = avProds.begin(), end = avProds.end(); iterator != end; ++iterator) {
		ASSERT_TEST((*iterator).getId() == temp_ids2[i] && (*iterator).getValue() == temp_values2[i]);
		i++;
	}
	return true;
}

bool testCloseReturning() {
	Factory factory=Factory();
	
	factory.closeReturningService();
	factory.closeReturningService();
	
	Product products[4];
	products[0]=Product(1,3);
	products[1]=Product(2,5);
	products[2]=Product(3,7);
	products[3]=Product(4,9);
	factory.startProduction(4, products, 1);
	factory.finishProduction(1);
	
	factory.startCompanyBuyer(2, 6, 2);
	
	sleep(1); // let the company finish the buying part
	
	factory.startSimpleBuyer(3);
	ASSERT_TEST(factory.finishSimpleBuyer(3) == 3);
	
	factory.startCompanyBuyer(1, 1, 4);
	ASSERT_TEST(factory.finishCompanyBuyer(4) == 0);
	
	list<Product> avProds=factory.listAvailableProducts();
	ASSERT_TEST(avProds.size() == 0);
	
	factory.startThief(17, 666);
	ASSERT_TEST(factory.finishThief(666) == 0);
	
	factory.openReturningService();
	factory.openReturningService();
	
	sleep(1); // let the company return it's 2 products
	
	avProds=factory.listAvailableProducts();
	ASSERT_TEST(avProds.size() == 2);
	
	factory.startThief(12, 555);
	ASSERT_TEST(factory.finishThief(555) == 2);
	
	ASSERT_TEST(factory.finishCompanyBuyer(2) == 2);
	
	return true;
}


bool testReusingIds() {
	Factory factory=Factory();
	
	Product products[3];
	products[0]=Product(1,3);
	products[1]=Product(2,5);
	products[2]=Product(3,7);
	factory.startProduction(1, products, 1);
	factory.finishProduction(1);
	factory.startProduction(1, products+1, 1);
	factory.finishProduction(1);
	factory.startProduction(1, products+2, 1);
	factory.finishProduction(1);

	factory.startCompanyBuyer(1, 10, 1);
	ASSERT_TEST(factory.finishCompanyBuyer(1) == 1);
	
	factory.startSimpleBuyer(1);
	ASSERT_TEST(factory.finishSimpleBuyer(1) == 2);
	
	factory.startThief(3, 1);
	ASSERT_TEST(factory.finishThief(1) == 2);
	
	return true;
}


bool testSync() {
	Factory factory=Factory();
	Product allProducts[TEST_SYNC_SIZE][TEST_SYNC_SIZE];
	int id = 1;
	for(int i=2; i<TEST_SYNC_SIZE; i++){
		for(int j=0; j<i; j++){
			allProducts[i][j]=Product(id++,i);
		}
	}
	
	for(int i=2; i<TEST_SYNC_SIZE; i++){
		Product* products = allProducts[i];
		factory.startProduction(i, products, i);
		factory.startCompanyBuyer(i, TEST_SYNC_SIZE, TEST_SYNC_SIZE+i);
		factory.startSimpleBuyer(10*TEST_SYNC_SIZE+i);
		factory.startThief(i/2, 100*TEST_SYNC_SIZE+i);
	}
	
	for(int i=2; i<TEST_SYNC_SIZE; i++){
		factory.finishProduction(i);
		factory.finishCompanyBuyer(TEST_SYNC_SIZE+i);
		factory.finishSimpleBuyer(10*TEST_SYNC_SIZE+i);
		factory.finishThief(100*TEST_SYNC_SIZE+i);
	}
	
	return true;
}

bool testStressTestSync() {
	for(int i=0; i<STRESS_TEST_SIZE; i++){
		if(!testSync()){
			return false;
		}
		
		if(i%100 == 0){
			printf("test loop #%d\n", i);
			fflush(stdout);
		}
	}
	return true;
}


int main (){
	RUN_TEST(testProduce);
	RUN_TEST(testTryBuyOne);
	RUN_TEST(testBuyProducts);
	RUN_TEST(testReturnProducts);
	RUN_TEST(testStandardRun);
	RUN_TEST(testOpenAndClose);
	RUN_TEST(testCloseReturning);
	RUN_TEST(testReusingIds);
	RUN_TEST(testStressTestSync); // if it freezes, that's probably mean you have a deadlock or someting
	return 0;
}






