#include "tests.cpp"
void testEmptyBook()
{
    LimitOrderBook book;
    
    assert(!book.bestBid().has_value());
    assert(!book.bestAsk().has_value());
    assert(!book.spread().has_value());
    
    std::cout << "testEmptyBook passed\n";
}

void testBasicAdd()
{
    LimitOrderBook book;
    
    book.add({1, Side::Buy, 9800, 100});
    book.add({2, Side::Buy, 9700, 200});
    
    book.add({3, Side::Sell, 9900, 150});
    book.add({4, Side::Sell, 10000, 300});
    
    assert(book.bestBid().value() == 9800);
    assert(book.bestAsk().value() == 9900);
    assert(book.spread().value() == 100);
    
    std::cout << "testBasicAdd passed\n";
}

void testMultipleOrdersAtPrice()
{
    LimitOrderBook book;
    book.add({1,Side::Buy, 2000, 100});
    book.add({2,Side::Buy, 2000, 150});
    book.add({3,Side::Buy, 2000, 120});
    
    auto quantity = book.quantityAtPrice(Side::Buy, 2000);
    
    assert(quantity.has_value());
    assert(quantity.value() == 370);
    std::cout << "testMultipleOrdersAtPrice passed\n";

}

void duplicateID()
{
    LimitOrderBook book;
    book.add({1, Side::Buy, 2000, 100});
    book.add({1, Side::Buy, 1234, 100});
    
    assert(book.bestBid() == 2000);
    
    auto quantity = book.quantityAtPrice(Side::Buy, 2000);
    assert(quantity.has_value());
    assert(quantity.value() == 100);
    
    auto quantity2 = book.quantityAtPrice(Side::Sell, 1234);
    assert(!quantity2.has_value());
    assert(quantity2 == std::nullopt);
    
    std::cout << "Duplicate ID passed\n";
}

void modifyTest()
{
    LimitOrderBook book;
    
    book.add({1, Side::Buy, 2000, 100});
    book.add({2, Side::Buy, 2000, 200});
    book.modify(1, 500);
    
    auto quantity = book.quantityAtPrice(Side::Buy, 2000);
    
    assert(quantity.has_value());
    assert(quantity.value() == 700);
    
    std::cout << "modifyTest passed\n";
    
}

void cancelTest()
{
    LimitOrderBook book;
    
    book.add({1, Side::Buy, 2000, 100});
    book.add({2, Side::Buy, 2000, 200});
    book.cancel(1);
    
    auto quantity = book.quantityAtPrice(Side::Buy, 2000);
    
    assert(quantity.has_value());
    assert(quantity.value() == 200);
    
    std::cout << "cancelTest passed\n";
    
}

void cancelUnknownID()
{
    LimitOrderBook book;
    
    book.add({1, Side::Buy, 2000, 100});
    book.add({2, Side::Buy, 2000, 200});
    book.cancel(3);
    
    auto quantity = book.quantityAtPrice(Side::Buy, 2000);
    
    assert(quantity.has_value());
    assert(quantity.value() == 300);
    
    std::cout << "cancelUnknownID passed\n";
    
}

void runAllTests()
{
    testEmptyBook();
    testBasicAdd();
    testMultipleOrdersAtPrice();
    duplicateID();
    modifyTest();
    cancelTest();
    cancelUnknownID();
    
    std::cout << "All tests passed!\n";
}
