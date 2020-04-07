#include "Start.h"

namespace command {

    void Start::exec(Command *cmd) {
        app_is_run = true;

        cout << "Try to start ["<<&app_is_run<<"]: " <<app_is_run<< endl;
    }

}