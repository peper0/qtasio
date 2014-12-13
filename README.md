qtasio
======

Implementation of QAbstractEventDispatcher that makes use of existing boost::asio::io_service object. It effecitvely allows to integrate asio-based asynchronous application with QT and run them in one thread.

## How to use?
Add qasioeventdispatcher.* to your project. Then add the following line before creating QApplication:
``` cpp
QApplication::setEventDispatcher(new QAsioEventDispatcher(my_io_service));
```

That's it. `QApplication::exec()` will run your io_service and use it for own events.
