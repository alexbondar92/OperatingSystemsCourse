
#ifndef PRODUCT_H_
#define PRODUCT_H_

class Product{
    int id;
    int value;
public:
    Product(){}
    Product(int id,int value){
        this->id = id;
        this->value = value;
    }
    int getId(){
        return id;
    }
    int getValue(){
        return value;
    }
};
#endif // PRODUCT_H_
