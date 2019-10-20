#ifndef FACTORY_H_
#define FACTORY_H_

#include <pthread.h>
#include <list>
#include "Product.h"


class pidToID{
public:
    pthread_t pid;
    int id;
//public:
    pidToID(pthread_t pid, int id) : pid(pid), id(id){}
    
    pthread_t getPid (){
        return this->pid;
    }
    
    int getId (){
        return this->id;
    }
    pidToID(){};
    
    bool operator==(const pidToID& p1){
        return p1.id == this->id;
    }
    
};


class Factory{
    
public:
    bool open_factory;
    bool returning_service;
    std::list<Product> products;
    std::list<std::pair<Product, int> > stolen_products;
    
    std::list<pidToID> production_requests;
    std::list<pidToID> buyers_requests;
    std::list<pidToID> companies_requests;
    std::list<pidToID> thief_requests;
    
    pthread_mutex_t lock_production_requests_list;
    pthread_mutex_t lock_products_list;
    pthread_mutex_t lock_stolen_products;
    pthread_mutex_t lock_buyer_requests_list;
    pthread_mutex_t lock_company_requests_list;
    pthread_mutex_t lock_thief_requests_list;
    
    
    pthread_mutex_t lock_buy_products;
    
    pthread_cond_t cond_nep;
    pthread_cond_t cond_open_factory;
    pthread_cond_t cond_return_service;
    
    pthread_mutexattr_t attr;
    
    int num_of_active_companies;            // change to private & GET/SET functions
    int num_of_active_thieves;
    
    Factory();
    ~Factory();
    
    void startProduction(int num_products, Product* products, unsigned int id);
    void produce(int num_products, Product* products);
    void finishProduction(unsigned int id);
    
    void startSimpleBuyer(unsigned int id);
    int tryBuyOne();
    int finishSimpleBuyer(unsigned int id);

    void startCompanyBuyer(int num_products, int min_value,unsigned int id);
    std::list<Product> buyProducts(int num_products);
    void returnProducts(std::list<Product> products,unsigned int id);
    int finishCompanyBuyer(unsigned int id);

    void startThief(int num_products,unsigned int fake_id);
    int stealProducts(int num_products,unsigned int fake_id);
    int finishThief(unsigned int fake_id);

    void closeFactory();
    void openFactory();
    
    void closeReturningService();
    void openReturningService();
    
    std::list<std::pair<Product, int> > listStolenProducts();
    std::list<Product> listAvailableProducts();

};
#endif // FACTORY_H_
