#pragma once

class SubSystem {
public:    
    /// @brief Return status of functions
    enum FunctionStatus {
        Ok, Err
    };

    /// @brief Inits this subsystem, this is different from constructor as 
    ///        init will only be called when hardware is ready. While constructor of a 
    ///        Subsystem may be called as soon as CPU is ready and up. 
    /// @return status of weather or not init() succeeds 
    virtual FunctionStatus init() noexcept {
        // default do nothing
        return FunctionStatus::Ok;
    };

    /// @brief Runs this subsystem for one cycle
    /// @return status of weather or not run() succeeds
    virtual FunctionStatus periodic() noexcept {
        // default do nothing
        return FunctionStatus::Ok;
    };
};

