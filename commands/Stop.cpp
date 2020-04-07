#include "Stop.h"


namespace command {

    void Stop::exec(Command *cmd) {
        app_is_run = false;
        cout << "Try to stop ["<<&app_is_run<<"]: "<< app_is_run << endl;
    }
}