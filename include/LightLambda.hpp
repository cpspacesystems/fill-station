#include <type_traits>

/// @brief A light Lambda holder for in place replacement of std::function
/// @tparam ReturnType the return type of this Lambda
/// @tparam ...Args argument type for this Lambda
template <typename ReturnType, typename... Args>
struct Lambda {
   using Signature = ReturnType(*)(void*, Args...);

   void* context = nullptr;
   Signature callback = nullptr;

   /// @brief Create a Lambda with function ptr and context set to nullptr
   Lambda() = default;
   /// @brief Create a Lambda
   /// @param context [lifetime object] the context/this struct void ptr
   /// @param callback [lifetime object] the lambda body function ptr
   Lambda(void* context, Signature callback) : context(context), callback(callback) {};
   Lambda(Signature callback) : callback(callback) {};
   
   /// @brief calls this Lambda
   /// @param ...args the arguments
   /// @return the return value of this Lambda 
   ReturnType operator()(Args... args) {
      this->callback(context, args...);
   };

   /// @brief check if this Lambda is callable
   explicit operator bool() const noexcept {
      return this->callback == nullptr;
   };
};