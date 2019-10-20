#include "Factory.h"
#include <pthread.h>


#include <iostream>
#include <assert.h>
#include <unistd.h>


// Initialize the mutexs & the other fields to zero
// in addition we initialize the conditional varibles
Factory::Factory(){
    pthread_mutexattr_init(&this->attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&this->lock_products_list, &this->attr);
    pthread_mutex_init(&this->lock_buy_products, &this->attr);
    pthread_mutex_init(&this->lock_stolen_products, &this->attr);
    pthread_mutex_init(&this->lock_buyer_requests_list, &this->attr);
    pthread_mutex_init(&this->lock_thief_requests_list, &this->attr);
    pthread_mutex_init(&this->lock_company_requests_list, &this->attr);
    pthread_mutex_init(&this->lock_production_requests_list, &this->attr);

    this->num_of_active_thieves = 0;
    this->num_of_active_companies = 0;
    
    pthread_cond_init(&cond_nep, NULL);
    pthread_cond_init(&cond_open_factory, NULL);
    pthread_cond_init(&cond_return_service, NULL);
    
    this->open_factory = true;
    this->returning_service = true;
}

// Destroys the mutexs & conditional varibles
Factory::~Factory(){
    pthread_mutex_destroy(&this->lock_products_list);
    pthread_mutex_destroy(&this->lock_buy_products);
    pthread_mutex_destroy(&this->lock_stolen_products);
    pthread_mutex_destroy(&this->lock_buyer_requests_list);
    pthread_mutex_destroy(&this->lock_thief_requests_list);
    pthread_mutex_destroy(&this->lock_company_requests_list);
    pthread_mutex_destroy(&this->lock_production_requests_list);
    
    pthread_cond_destroy(&this->cond_nep);
    pthread_cond_destroy(&this->cond_open_factory);
    pthread_cond_destroy(&this->cond_return_service);
    
    pthread_mutexattr_destroy(&this->attr);
}


// ===============================================================================================
// ============================= Start Production help Functions =================================
// ===============================================================================================

// Class for a produce function.
// that is inorder to send the relevent data to the thread main function
// the data that is needed is the number of products, the pointer to the products array
// and the pointer of the relevent Factory class.
// there is a constractor, get, set functions(nothing special).
class __produce_t{
    int num_products;
    Product* products;
    Factory* current_factory;
public:
    __produce_t(int num_products, Product* products, Factory* current_factory) : num_products(num_products), products(products) , current_factory(current_factory){}
    
    int getNumProducts(){
        return this->num_products;
    }
    Product* getProducts(){
        return this->products;
    }
    Factory* getFactory(){
        return this->current_factory;
    }
};

// The main thread function for the startProduction
// it is calling to the produce function with the relevent args and the relevent Factory class.
void* __produce(void* ptr){
    __produce_t* ptr2 = (__produce_t*)ptr;
    ptr2->getFactory()->Factory::produce(ptr2->getNumProducts(), ptr2->getProducts());
    delete (__produce_t*)ptr;
    return NULL;
}
// ===============================================================================================
// ===============================================================================================
// ===============================================================================================

// Create a new thread with the __produce function and the __produce_t* data.
// in addition we inserting the pid of the thread to the list.
// we lock the lock of production requests list.
void Factory::startProduction(int num_products, Product* products,unsigned int id){
    __produce_t* tmp = new __produce_t(num_products , products, this);
    
    pthread_t pid;
    
    if(pthread_create(&pid, NULL, __produce, (void*)tmp)){
        // ================================================== To Delete
//        fprintf(stderr, "Error joining thread, At the startProduction Function\n");
    }
    
    pidToID new_request = pidToID(pid, id);
    
    pthread_mutex_lock(&this->lock_production_requests_list);
    this->production_requests.push_front(new_request);
    pthread_mutex_unlock(&this->lock_production_requests_list);
}

// Insert the new products into the products list, in addition
// after the is insertion of all the products we broadcast signal for the cond_nep conditional varible
// we locking the lock of the products list.
void Factory::produce(int num_products, Product* products){
    pthread_mutex_lock(&this->lock_products_list);
    
//    std::cout << "thread with id" << pthread_self() << "got the products_list lock in produce\n"; // delete

    
    for (int i=0; i< num_products; i++){
        this->products.push_front(products[i]);
    }
    pthread_cond_broadcast(&this->cond_nep);
    pthread_mutex_unlock(&this->lock_products_list);
}

// Joining the finish thread to the main thread.
// we locking the lock of the products list.
void Factory::finishProduction(unsigned int id){
    pthread_mutex_lock(&this->lock_production_requests_list);
    
    std::list<pidToID>::iterator itr = this->production_requests.begin();
    while (itr != this->production_requests.end() && itr->getId() != id){
        ++itr;
    }
//    std::list<pidToID>::iterator itr = find(this->production_requests.begin(),
//                                                 this->production_requests.end(), pidToID(0, id));
    if (itr == this->production_requests.end()){
        return;
    }
    
    if (pthread_join(itr->getPid(), NULL)){
        // ================================================== To Delete
        fprintf(stderr, "Error joining thread, At the startProduction Function\n");
    }
    
    this->production_requests.erase(itr);
    pthread_mutex_unlock(&this->lock_production_requests_list);
}


// ===============================================================================================
// ============================ Start Simple Buyer help Functions ================================
// ===============================================================================================

// The main thread function for the startSimpleBuyer
// it is calling to the tryBuyOne function with the relevent args and the relevent Factory class.
void* __tryBuyOne(void* ptr){
    Factory* ptr2 = (Factory*)ptr;
    int* result = new int;
    *result = ptr2->tryBuyOne();
    return result;
}

// ===============================================================================================
// ===============================================================================================
// ===============================================================================================

// Create a new thread with the __tryBuyOne function and pointer to the relevent Factory.
// in addition we inserting the pid of the thread to the list.
// we lock the lock of buyer requests list.
void Factory::startSimpleBuyer(unsigned int id){
    pthread_t pid;
    if(pthread_create(&pid, NULL, __tryBuyOne, this)){
        // ================================================== To Delete
//        fprintf(stderr, "Error pthread create, At the startSimpleBuyer Function\n");
    }
    
    pidToID new_request = pidToID(pid, id);
    
    pthread_mutex_lock(&this->lock_buyer_requests_list);
    this->buyers_requests.push_front(new_request);
    pthread_mutex_unlock(&this->lock_buyer_requests_list);
}

// trying to buy one product from the factory, if the lock of the products list is locked or the factory is
// closed than the function returns -1.
// we truing to lock the lock of the products list.
int Factory::tryBuyOne(){
    if (this->open_factory == false){
        return -1;
    }
    if (this->products.size() == 0){
        return -1;
    }
    if (pthread_mutex_trylock(&this->lock_products_list) == 0){
        
//        std::cout << "thread with id" << pthread_self() << "got the products_list lock in tryBuyOne\n"; // delete
        
        std::list<Product>::iterator itr = this->products.end();
        itr--;
        int result = itr->getId();
        if (this->products.size() != 0){
            this->products.pop_back();
        } else {
            int i =0;
            i++;
        }
        pthread_mutex_unlock(&this->lock_products_list);
        return result;
    }
    return -1;
}

// Joining the finish thread to the main thread.
// we locking the lock of the buyer requests list.
int Factory::finishSimpleBuyer(unsigned int id){
    pthread_mutex_lock(&this->lock_buyer_requests_list);
    
    std::list<pidToID>::iterator itr = this->buyers_requests.begin();
    while (itr != this->buyers_requests.end() && itr->getId() != id){
        ++itr;
    }
    
//    std::list<pidToID>::iterator itr = find(this->buyers_requests.begin(),
//                                                 this->buyers_requests.end(), pidToID(0, id));
    if (itr == this->buyers_requests.end()){
        // ================================================== To Delete
//        printf("ERROR!!, at function finishSimpleBuyer\n");
        return -2;
    }
    
    int* result = NULL;
    if (pthread_join(itr->getPid(), (void**)&result)){
        // ================================================== To Delete
//        fprintf(stderr, "Error joining thread, At the finishSimpleBuyer Function\n");
    }
    this->buyers_requests.erase(itr);
    pthread_mutex_unlock(&this->lock_buyer_requests_list);

    int result2 = *result;
    delete result;
    return result2;
}


// ===============================================================================================
// ================================== Start Company help Functions ===============================
// ===============================================================================================

// Class for a __buyProducts function.
// that is inorder to send the relevent data to the thread main function
// the data that is needed is the number of products, the minimum value to products
// and the pointer of the relevent Factory class.
// there is a constractor, get, set functions(nothing special).
class __buyProducts_t{
    int num_products;
    int min_value;
    Factory* current_factory;
public:
    __buyProducts_t(int num_products, int min_value, Factory* current_factory) : num_products(num_products), min_value(min_value), current_factory(current_factory){}
    
    int getNumProducts(){
        return this->num_products;
    }
    Factory* getFactory(){
        return this->current_factory;
    }
    int getMinValue(){
        return this->min_value;
    }
    __buyProducts_t(){
    };
};

// The main thread function for the startCompanyBuyer
// it is calling to the buyProducts function with the relevent args and the relevent Factory class.
// after that we return the products that is less than the minimum value and returning them by the
// returnProducts fucntion.
// we using the company requests list lock.
void* __buyProducts(void* ptr){
    int num_products = ((__buyProducts_t*)ptr)->getNumProducts();
    Factory* tmp = ((__buyProducts_t*)ptr)->getFactory();
    
//    pthread_mutex_lock(&tmp->lock_company_requests_list);
    
    std::list<Product> buyList = tmp->Factory::buyProducts(num_products);
    
    pthread_mutex_lock(&tmp->lock_company_requests_list);
    tmp->num_of_active_companies--;
    pthread_mutex_unlock(&tmp->lock_company_requests_list);
    
    std::list<Product> returnList;
    
    int* returned_products = new int;
    *returned_products = 0;
    
    std::list<Product>::iterator itr = buyList.begin();
    while (itr != buyList.end()) {
        if (itr->getValue() < ((__buyProducts_t*)ptr)->getMinValue()){
            returnList.push_back(Product(itr->getId(),itr->getValue()));
            (*returned_products)++;
        }
        itr++;
    }
    if (returnList.size() != 0){
        tmp->returnProducts(returnList, 0);
    }
    
    delete (__buyProducts_t*)ptr;
    return returned_products;
}

// ===============================================================================================
// ===============================================================================================
// ===============================================================================================

// Create a new thread with the __buyProducts function and relevent data by the __buyProducts_t object.
// in addition we inserting the pid of the thread to the list.
// we lock the lock of company requests list.
void Factory::startCompanyBuyer(int num_products, int min_value,unsigned int id){
    pthread_t pid;
    __buyProducts_t* tmp = new __buyProducts_t(num_products, min_value, this);
    
    pthread_mutex_lock(&this->lock_company_requests_list);
    this->num_of_active_companies++;
    pthread_mutex_unlock(&this->lock_company_requests_list);
    
    if(pthread_create(&pid, NULL, __buyProducts, (void*)tmp)){
        // ================================================== To Delete
        //        fprintf(stderr, "Error pthread create, At the startCompanyBuyer Function\n");
    }
    
    pidToID new_request = pidToID(pid, id);
    
    pthread_mutex_lock(&this->lock_company_requests_list);
    this->companies_requests.push_front(new_request);
    pthread_mutex_unlock(&this->lock_company_requests_list);
}

// Exporting the number of needed products to the returning list
// after the is exporting of all the products we broadcast signal for the cond_nep conditional varible
// we locking the lock of the products list.
std::list<Product> Factory::buyProducts(int num_products){
    std::list<Product> return_list;
    
    pthread_mutex_lock(&this->lock_products_list);
    
//    std::cout << "thread with id" << pthread_self() << "got the products_list lock in buyProducts (1)\n"; // delete
    
//    bool flag = false;
//    pthread_mutex_lock(&this->lock_thief_requests_list);
    while (this->products.size() < num_products || this->num_of_active_thieves != 0 || this->open_factory == false){
//        if (flag == false){
//            pthread_mutex_unlock(&this->lock_thief_requests_list);
//            flag = true;
//        }
        pthread_cond_wait(&this->cond_nep, &this->lock_products_list);
    }
    
//    std::cout << "thread with id" << pthread_self() << "got the products_list lock in buyProducts (2)\n"; // delete
    
    for (int i = 0; i < num_products; i++){
        std::list<Product>::iterator itr = this->products.end();
        itr--;
        return_list.push_back(Product(itr->getId(), itr->getValue()));
        this->products.pop_back();
    }
    
    pthread_cond_broadcast(&this->cond_nep);                // =================== maybe to delete......
    pthread_mutex_unlock(&this->lock_products_list);
    return return_list;
}

// Insert the returned products into the products list, in addition
// after the is insertion of all the products we broadcast signal for the cond_nep conditional varible
// we locking the lock of the products list.
void Factory::returnProducts(std::list<Product> products,unsigned int id){
    pthread_mutex_lock(&this->lock_products_list);
    
//    std::cout << "thread with id" << pthread_self() << "got the products_list lock in returnProducts (1) \n"; // delete
    
    while (this->returning_service == false){
        pthread_cond_wait(&this->cond_return_service, &this->lock_products_list);
    }
    
//    std::cout << "thread with id" << pthread_self() << "got the products_list lock in returnProducts (2)\n"; // delete
    
    while(products.size() != 0){
        this->products.push_front(Product(products.begin()->getId(), products.begin()->getValue()));
        products.pop_front();
    }
    
    pthread_cond_broadcast(&this->cond_nep);
    pthread_mutex_unlock(&this->lock_products_list);
}

// Joining the finish thread to the main thread.
// we locking the lock of the company requests list.
int Factory::finishCompanyBuyer(unsigned int id){
    pthread_mutex_lock(&this->lock_company_requests_list);
    
    std::list<pidToID>::iterator itr = this->companies_requests.begin();
    while (itr != this->companies_requests.end() && itr->getId() != id){
        ++itr;
    }
    
//    std::list<pidToID>::iterator itr = find(this->companies_requests.begin(),
//                                                 this->companies_requests.end(), pidToID(0, id));
    if (itr == this->companies_requests.end()){
        // ================================================== To Delete
        printf("ERROR!!, at function finishCompanyBuyer\n");
        pthread_mutex_unlock(&this->lock_company_requests_list);
        return -2;
    }
    pthread_mutex_unlock(&this->lock_company_requests_list);
    
    int* result = NULL;
    if (pthread_join(itr->getPid(), (void**)&result)){
        // ================================================== To Delete
//        fprintf(stderr, "Error joining thread, At the finishCompanyBuyer Function\n");
    }
    pthread_mutex_lock(&this->lock_company_requests_list);
    this->companies_requests.erase(itr);
    pthread_mutex_unlock(&this->lock_company_requests_list);
    
    int result2 = *result;
    if (result != NULL){
        delete result;
    }
    result = NULL;
    return result2;
}

// ===============================================================================================
// ================================== Start Company help Functions ===============================
// ===============================================================================================

// Class for a __stealProducts function.
// that is inorder to send the relevent data to the thread main function
// the data that is needed is the number of products, the fake id of the thief
// and the pointer of the relevent Factory class.
// there is a constractor, get, set functions(nothing special).
class __stealProducts_t{
    int num_products;
    int fake_id;
    Factory* current_factory;
public:
    __stealProducts_t(int num_products, int fake_id, Factory* current_factory) : num_products(num_products), fake_id(fake_id), current_factory(current_factory){}
    
    int getNumProducts(){
        return this->num_products;
    }
    int getFakeID(){
        return this->fake_id;
    }
    Factory* getFactory(){
        return this->current_factory;
    }
    __stealProducts_t(){
    };
};

// The main thread function for the startThief
// it is calling to the stealProducts function with the relevent args and the relevent Factory class.
// we using the thief requests list lock.
void* __stealProducts(void* ptr){
    int num_products = ((__stealProducts_t*)ptr)->getNumProducts();
    Factory* tmp = ((__stealProducts_t*)ptr)->getFactory();
    int fake_id = ((__stealProducts_t*)ptr)->getFakeID();
    int* return_value = new int;
    *return_value = tmp->Factory::stealProducts(num_products, fake_id);
    
    pthread_mutex_lock(&tmp->lock_thief_requests_list);
    tmp->num_of_active_thieves--;
    pthread_mutex_unlock(&tmp->lock_thief_requests_list);
    
    pthread_cond_broadcast(&tmp->cond_nep);
    delete (__stealProducts_t*)ptr;
    return return_value;
}

// ===============================================================================================
// ===============================================================================================
// ===============================================================================================

// Create a new thread with the __stealProducts function and relevent data by the __stealProducts_t object.
// in addition we inserting the pid of the thread to the list.
// we lock the lock of thief requests list.
void Factory::startThief(int num_products,unsigned int fake_id){
    pthread_t pid;
    __stealProducts_t* tmp = new __stealProducts_t(num_products, fake_id, this);
    
    pthread_mutex_lock(&this->lock_thief_requests_list);
    this->num_of_active_thieves++;
    pthread_mutex_unlock(&this->lock_thief_requests_list);
    
    if(pthread_create(&pid, NULL, __stealProducts, (void*)tmp)){
        // ================================================== To Delete
//        fprintf(stderr, "Error pthread create, At the startThief Function\n");
    }
    
    pidToID new_request = pidToID(pid, fake_id);
    
    pthread_mutex_lock(&this->lock_thief_requests_list);
    this->thief_requests.push_front(new_request);
    pthread_mutex_unlock(&this->lock_thief_requests_list);
}

// Poping the number of needed products from the products list, if there is less products the number of
// possible products will be poped.
// after the is poping outthe products we broadcast signal for the cond_nep conditional varible
// we locking the lock of the products list.
int Factory::stealProducts(int num_products,unsigned int fake_id){
    pthread_mutex_lock(&this->lock_products_list);
    
//    std::cout << "thread with id" << pthread_self() << "got the products_list lock in stealProducts (1) \n"; // delete
    
    while (this->open_factory == false){
        pthread_cond_wait(&this->cond_nep, &this->lock_products_list);
    }
    
//    std::cout << "thread with id" << pthread_self() << "got the products_list lock in stealProducts (2) \n"; // delete
    
    
    int result = 0;
    
    for (int i = 0; i < num_products && this->products.size(); i++){
        std::list<Product>::iterator itr = this->products.end();
        itr--;
        this->stolen_products.push_back(std::pair<Product, int>(Product(itr->getId(), itr->getValue()),fake_id));
        this->products.pop_back();
        result++;
    }
    
    pthread_cond_broadcast(&this->cond_nep);
    pthread_mutex_unlock(&this->lock_products_list);
    return result;
}

// Joining the finish thread to the main thread.
// we locking the lock of the thief requests list.
int Factory::finishThief(unsigned int fake_id){
    pthread_mutex_lock(&this->lock_thief_requests_list);
    
    std::list<pidToID>::iterator itr = this->thief_requests.begin();
    while (itr != this->thief_requests.end() && itr->getId() != fake_id){
        ++itr;
    }
    
//    std::list<pidToID>::iterator itr = find(this->thief_requests.begin(),
//                                                 this->thief_requests.end(), pidToID(0, fake_id));
    if (itr == this->thief_requests.end()){
//         ================================================== To Delete
        printf("ERROR!!, at function finishThief\n");
        pthread_mutex_unlock(&this->lock_thief_requests_list);
        return -2;
    }
    pthread_mutex_unlock(&this->lock_thief_requests_list);
    
    int* result = NULL;
    if (pthread_join(itr->getPid(), (void**)&result)){
        // ================================================== To Delete
//        fprintf(stderr, "Error joining thread, At the finishThief Function\n");
    }
    pthread_mutex_lock(&this->lock_thief_requests_list);
    this->thief_requests.erase(itr);
    pthread_mutex_unlock(&this->lock_thief_requests_list);
    
    int result2 = *result;
    delete result;
    pthread_cond_broadcast(&this->cond_nep);
    return result2;
}

// closing the factory by chenging the open_factory flag to false.
void Factory::closeFactory(){
    this->open_factory = false;
}

// opening the factory by chenging the open_factory flag to true.
// in addition with broadcasting signal for the relevent conditional varibles.
void Factory::openFactory(){
    this->open_factory = true;
    pthread_cond_broadcast(&this->cond_open_factory);
    pthread_cond_broadcast(&this->cond_return_service);
    pthread_cond_broadcast(&this->cond_nep);
}

// closing the returning service by chenging the returning_service flag to false.
void Factory::closeReturningService(){
    this->returning_service = false;
}

// opening the returning service by chenging the returning_service flag to true.
// in addition with broadcasting signal for the relevent conditional varibles.
void Factory::openReturningService(){
    this->returning_service = true;
    pthread_cond_broadcast(&this->cond_return_service);
    pthread_cond_broadcast(&this->cond_nep);
}

// returning a copy of the list of stolen products.
std::list<std::pair<Product, int>> Factory::listStolenProducts(){
    pthread_mutex_lock(&this->lock_stolen_products);
    
    std::list<std::pair<Product, int>>tmp = this->stolen_products;
    
    pthread_mutex_unlock(&this->lock_stolen_products);
    return tmp;
}

// returning a copy of the list of the availavle products in the factory.
std::list<Product> Factory::listAvailableProducts(){
    std::list<Product> return_list;
    pthread_mutex_lock(&this->lock_products_list);
    
//    std::cout << "thread with id" << pthread_self() << "got the products_list lock in listAvailableProducts \n"; // delete
    
    for(auto &&i : this->products){
        return_list.push_front(Product(i.getId(), i.getValue()));
    }
    
    pthread_mutex_unlock(&this->lock_products_list);
    return return_list;
}
