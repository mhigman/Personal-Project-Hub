#ifndef MIT_App_Inventor_Parser
#define MIT_App_Inventor_Parser

#include <iterator>
#include <Arduino.h>
#include <Servo.h>
#include <AccelStepper.h>
#include <ArduinoJson.h>
#include "ArduinoBLE.h"


/**
 * @brief Namespace to store parser and other utilities.
 */
namespace AppInventor {

  /**
   * @brief A generic implementation of an enum to store the type of a certain block.
   */
  enum BasicDataType {
    INT = 0,
    DOUBLE = 1,
    BOOL = 2,
    //STRING = 3,
    CHAR = 4,
    LIST = 5,
  };


  /**
   * @brief An enum to quickly distinguish between different types of communication methods.
   */
  enum CommunicationMethod {
    COMM_SERIAL = 0,
    COMM_BLUETOOTH = 1,
    COMM_WIFI = 2
  };



  /**
   * @brief An enum to wrap command codes for the interpreter and make them more readable.
   * @details This command code enum should be added onto first before programming any new
   * functionality into the parser. This allows for someone to plan out what commands they want
   * to add. 
   * @note Now, when writing a new block, be sure to include a Doxygen comment listing it's intended purpose,
   * JSON schema, and any TODO's for each command.
   */
  enum Subroutine {
    /**
     * @brief A generic block
     * @note This block would mostly be used for debugging reasons. If a command code in a block
     * isn't specified, it will return this block.
     */
    GENERIC_BLOCK = 0,
    /**
     * @brief A block to implement a for loop that counts like a range() object in Python.
     * @details JsonObject -> double means "a JSON object that returns a double when parsed", and
     * JsonObject -> actions means "a JSON object that contains a list of actions (like the top-level JSON statement)"
     * @param start Type: JsonObject -> double = The starting value for the for loop, inclusive.
     * @param stop Type: JsonObject -> double = The threashold for the loop, exclusive.
     * @param step Type: JsonObject -> double = The increment of the iterator variable on each iteration.
     * @param action Type: JsonObject -> actions = The commands to execute in the for loop.
     * @note To access the iterator variable for this loop, simply use a variable call instruction
     * with an "iter_range" key and a value of "i" + an uppercase character whose code corresponds to the "depth" of the for loop.
     * 
     * (i.e. "iA" = depth of 0, "iB" = depth of 1, etc.)
     */
    FOR_RANGE = 1,
    /**
     * @brief A block to implement a forEach loop.
     * @details JsonArray<list_type> is an array of objects with a data type defined by list_type
     * @param list_type Type: BasicDataType = A number that corresponds to a type via the BasicDataType enum.
     * @param list Type: JsonObject -> JsonArray<list_type> = The array to iterate over.
     * @param action Type JsonObject -> actions = The commands to execute in the for loop.
     * @note To access the iterator variable for this loop, simply use a variable call instruction
     * with an "iter" key and a value of "i" + an uppercase character whose code corresponds to the "depth" of the for loop.
     * Accessing this iterator variable is very similar to the last case.
     * 
     * @note HOWEVER, if something is "int like" (i.e double, int, and unsigned char), the uppercase character may be offset 
     * since all three of these types are stored in the same buffer. If the iterator type is int, the starting letter (when depth = 0)
     * is 'K'. If the iterator type is unsigned char, the starting letter is 'U'.
     */
    FOR_LIST = 2,
    /**
     * @brief A block to implement a while loop.
     * @param condition Type: JsonObject -> bool = The condition to check for in every iteration of the loop.
     * @param action Type: JsonObject -> actions = The commands to execute in the while loop.
     */
    WHILE = 3,
    /**
     * @brief A block to implement an if statement.
     * @param condition Type: JsonObject -> bool = The condition to check in if statement.
     * @param action Type: JsonObject -> actions = The commands to execute in the if statment.
     */
    IF = 4,
    /**
     * @brief A block to implement an if else statement.
     * @param condition Type: JsonObject -> bool = The condition to check in if else statement.
     * @param action1 Type: JsonObject -> actions = The commands to execute in the if statment.
     * @param action2 Type: JsonObject -> actions = The commands to exectue in the else statement.
     */
    IF_ELSE = 5,
    /**
     * @brief A block to implement an if elif else statement.
     * @param condition Type: JsonObject -> bool = The condition to check in if elif else statement.
     * @param action1 Type: JsonObject -> actions = The commands to execute in the if statment.
     * @param action2 Type: JsonObject -> actions = The commands to execute in the else if statement.
     * @param action3 Type: JsonObject -> actions = The commands to execute in the else statement.
     */
    IF_ELIF_ELSE = 6,
    /**
     * @brief A block to move a Servo motor.
     * @param id Type: unsigned char = The id (or position in the Servo buffer as declared by the INIT_SERVO command) 
     * of the Servo.
     * @param degrees Type: JsonObject -> int = A degree value for the servo to turn towards.
     */
    MOVE_SERVO_MOTOR = 7,
    /**
     * @brief A block to move a Stepper motor.
     * @param id Type: unsigned char = The id (or position in the AccelStepper buffer as declared by the INIT_STEPPER command)
     * of the Stepper
     * @param position Type: JsonObject -> int = The number of steps the stepper should move. 
     */
    MOVE_STEPPER_MOTOR = 8,
    //etc. TODO: add more motor movement options
    /**
     * @brief A block to turn on a digital pin.
     * @param number Type: JsonObject -> unsigned char = The pin to power
     */
    DIGITAL_SWITCH_UP = 9,
    /**
     * @brief A block to turn off a digital pin.
     * @param number Type: JsonObject -> unsigned char = The pin to not power
     */
    DIGITAL_SWITCH_DOWN = 10,
    /**
     * @brief A block to check a digital pin's status.
     * @param number Type: JsonObject -> unsigned char = The pin to read
     * @return Type: unsigned char = The status of the pin as defined by Arduino's PinStatus enum.
     */
    READ_FROM_PORT_DIGITAL = 11,
    /**
     * @brief A block to check a analog pin's status.
     * @param number Type: JsonObject -> unsigned char = The pin to read
     * @return Type: unsigned char = The single byte value of the analog port
     */
    READ_FROM_PORT_ANALOG = 12,
    /**
     * @brief A block to write to an analog pin.
     * @param number Type: JsonObject -> unsigned char = The pin to store the number
     * @param value Type: JsonObject -> unsigned char = The value to store in the pin
     */
    WRITE_TO_PORT_ANALOG = 13,
    /**
     * @brief A block to create or overwrite a variable
     * @details The data type "type" is the data type specified by the BasicDataType parameter type.
     * @param type Type: BasicDataType = The type of the new variable (needed to partition buffers for types)
     * @param value Type: JsonObject -> type = The value to assign to the variable.
     * @note There is an optional parameter called "name", which works similarly to VARIABLE_CALL, but instead of returning
     * the variable, it overwrites that position in the buffer to a different value.
     */
    VARIABLE_SET = 14,
    //etc. TODO: add any more functionality I think of
    /**
     * @brief A wrapper block for a literal value. This block is one of the only "top-level" blocks.
     * @param value: JsonObject -> any = The literal value, literally
     * @return value
     */
    LITERAL = 15,
    /**
     * @brief A block to move a dc motor with a set power
     * @param id Type: unsigned char = The id (or position in the DCMotor buffer as declared by the INIT_MOTOR command)
     * of the DCMotor
     * @param speed Type: JsonObject -> int = The speed of the motor, ranging from -255 to 255
     */
    MOVE_DC_MOTOR = 16,
    /**
     * @brief A block to grab a variable
     * @param name Type: unsigned character = The name (or position in a buffer as declared by the VARIABLE_SET command)
     * of any given variable.
     * @return The returned value of the variable. The type of this call depends on the type coersion of the commands that made
     * the VARIABLE_CALL
     * @note "int like" types, similar to iterator variables, are stored in the same buffer. Varaibles with type int start at index
     * 0, variables with type double start at index 20, and variables with type unsigned char start at index 60.
     */
    VARIABLE_CALL = 23,
    /**
     * @todo Get more maths operations up and running (not hard)
     */
    /**
     * @param arg1 Type: JsonObject -> "int like" = first thing
     * @param arg2 Type: JsonObject -> "int like" = next thing
     * @return Sum
     */
    ADD = 24,
    /**
     * @param arg1 Type: JsonObject -> "int like" = first thing
     * @param arg2 Type: JsonObject -> "int like" = next thing
     * @return Difference
     */
    SUBTRACT = 25,
    /**
     * @param arg1 Type: JsonObject -> "int like" = first thing
     * @param arg2 Type: JsonObject -> "int like" = next thing
     * @return Product
     */
    MULTIPLY = 26,
    /**
     * @param arg1 Type: JsonObject -> "int like" = first thing
     * @param arg2 Type: JsonObject -> "int like" = next thing
     * @return Quotient
     */
    DIVIDE = 27,
    /**
     * @param arg1 Type: JsonObject -> bool = first thing
     * @param arg2 Type: JsonObject -> bool = next thing
     * @return And operation between two inputs
     */
    AND = 28,
    /**
     * @param arg1 Type: JsonObject -> bool = first thing
     * @return Not operation on input
     */
    NOT = 29,
    /**
     * @param arg1 Type: JsonObject -> bool = first thing
     * @param arg2 Type: JsonObject -> bool = next thing
     * @return Or operation between two inputs
     */
    OR = 30,
    /**
     * @param arg1 Type: JsonObject -> double (or any "int like") = first thing
     * @param arg2 Type: JsonObject -> double (or any "int like") = next thing
     * @return true if arg1 > arg2, false otherwise
     */
    GREATER_THAN = 31,
    /**
     * @param arg1 Type: JsonObject -> double (or any "int like") = first thing
     * @param arg2 Type: JsonObject -> double (or any "int like") = next thing
     * @return true if arg1 < arg2, false otherwise
     */
    LESS_THAN = 32,
    /**
     * @param arg1 Type: JsonObject -> double (or any "int like") = first thing
     * @param arg2 Type: JsonObject -> double (or any "int like") = next thing
     * @return true if arg1 != arg2, false otherwise
     */
    NOT_EQUAL_TO = 33,
    /**
     * @param arg1 Type: JsonObject -> double (or any "int like") = first thing
     * @param arg2 Type: JsonObject -> double (or any "int like") = next thing
     * @return true if arg1 >= arg2, false otherwise
     */
    GREATER_THAN_EQUAL = 34,
    /**
     * @param arg1 Type: JsonObject -> double (or any "int like") = first thing
     * @param arg2 Type: JsonObject -> double (or any "int like") = next thing
     * @return true if arg1 <= arg2, false otherwise
     */
    LESS_THAN_EQUAL = 35,
    /**
     * @param arg1 Type: JsonObject -> double (or any "int like") = first thing
     * @param arg2 Type: JsonObject -> double (or any "int like") = next thing
     * @return true if arg1 == arg2, false otherwise
     */
    EQUAL_TO = 36,
    /**
     * @brief A command to send numerical data, delimited by ',', to the input device.
     * @param method Type: CommunicationMethod = The way to send data; refer to the
     * CommunicationMethod enum for more information.
     * @param arg1 Type: JsonObject -> double = First number to send \n
     * . \n
     * . \n
     * . \n
     * @param argn Type: JsonObject -> double = nth number to send
     * @note This command takes in a varadic number arguments, with the key name of
     * each varadic argument being irrelevant.
     */
    SEND = 37,
    /**
     * @todo Implement, then document
     */
    RECEIVE = 38,
    /**
     * @brief A block to initialize a Servo as a global variable
     * @param port Type: JsonObject -> unsigned char = The port the Servo object should write to.
     * @note There is an optional parameter called "name", which works similarly to VARIABLE_CALL, but instead of returning
     * the variable, it overwrites the Servo port of that position in the buffer.
     */
    INIT_SERVO = 39,
    /**
     * @brief A block to initialize an AccelStepper instance as a global variable.
     * @param pin1 Type: JsonObject -> unsigned char = The step pin
     * @param pin2 Type: JsonObject -> unsigned char = The direction pin
     * @note There is an optional parameter called "name", which works similarly to VARIABLE_CALL, but instead of returning
     * the variable, it overwrites the AccelStepper object of that position in the buffer.
     * @note This command assumes the stepper motor is being controlled by a stepper driver.
     * @todo Implement this for different types of stepper motors.
     */
    INIT_STEPPER = 40,
    /**
     * @brief A block to wait
     * @param time Type: JsonObject -> int = The time (in ms) to delay
     */
    WAIT = 41,
    /**
     * @brief A block to initialize a DCMotor instance as a global variable
     * @param pin1 Type: JsonObject -> unsigned char = The first pin
     * @param pin2 Type: JsonObject -> unsigned char = The second pin
     * @note There is an optional parameter called "name", which works similarly to VARIABLE_CALL, but instead of returning
     * the variable, it overwrites the DCMotor object of that position in the buffer.
     */
    INIT_MOTOR = 42

    //etc. TOOD add more functionality
  };

  enum ArrayStatus{
    ARRAY
  };

  /**
   * @brief A depricated enum to identify a String type in a method overload
   * @details 
   */

  enum StringStatus{
    STR
  };

  enum InIterator{
    NO = 0,
    YES = 1
  };


  class DCMotor {
    public:
      pin_size_t pin1;
      pin_size_t pin2;

      DCMotor();
      DCMotor(pin_size_t p1, pin_size_t p2);
      void power(pin_size_t speed);
  };


  class Parser {
    public:


      /// @note - Important presupposition for variable names:
      /// Preprocessing will "replace" with unique, single byte ids
      /// (i.e. a, b, c, d, e, f, g, h, i, j)


      
      double buffer_int_like[60];
      int int_index = 0;
      int double_index = 20;
      int char_index = 40;
      

      bool buffer_bool[20];
      int bool_index = 0;

      Servo buffer_servo[10];
      int servo_index = 0;

      AccelStepper buffer_stepper[10];
      int stepper_index = 0;

      DCMotor buffer_motor[10];
      int motor_index = 0;


      JsonArray buffer_list[30];
      int list_index = 0;

      /**
       * @note We need to determing the relative depth and type of
       * iteration variables at "complie" time (JS is easier to work with)
       */



      double buffer_range_iter[10];
      int range_iter_index = -1;

      double buffer_element_int_like[30];
      int element_double_index = -1;
      int element_int_index = 9;
      int element_char_index = 19; 

      bool buffer_element_bool[10];
      int element_bool_index = -1;


      JsonArray buffer_element_list[10];
      int element_list_index = -1;

      BLEStringCharacteristic* writePacket = nullptr; 
              
      Parser();


      void prepareCommands(char (&command_feed)[3000], BLEStringCharacteristic* packet = nullptr);

      /**
       * @todo Add optional types for easier error handling. I'm currently using options lite rn.
       */


      /**
     * @brief A function to parse a list of commands in a scope
     * @param comms The scope to parse commands in.
     * @param in_iterator A deprecated parameter to determine a type of scope.
     * @note Command names in the comms parameter don't matter.
     * @todo in_iterator is depracated, so it should be deleted
     */
      void parseCommands(JsonObject comms, InIterator in_iterator = InIterator::NO);
      /**
       * @brief Parse a command that shouldn't return anything
       * @param command_info A JSON object that provides information about a command.
       * @note When implementing other data types, don't add a default parameter, as 
       * using the default parameter may confuse the complier when deciding which 
       * method overload to use.
       */
      void parseCommand(JsonObject command_info);
      /**
       * @brief Parse a command that should return a boolean value
       * @param command_info A JSON object that provides information about a command.
       * @param type_signifier A boolean to return as a default value to the function.
       * @return boolean type from the resulting operation or the default value
       */
      bool parseCommand(JsonObject command_info, bool type_signifier);
      /**
       * @brief Parse a command that should return an integer (2-byte) value
       * @param command_info A JSON object that provides information about a command.
       * @param type_signifier An integer to return as a default value to the function.
       * @return integer type from the resulting operation or the default value
       */
      int parseCommand(JsonObject command_info, int type_signifier);
      /**
       * @brief Parse a command that should return an double (4-byte) value
       * @param command_info A JSON object that provides information about a command.
       * @param type_signifier A double to return as a default value to the function.
       * @return double type from the resulting operation or the default value
       */
      double parseCommand(JsonObject command_info, double type_signifier);
      /**
       * @brief Parse a command that should return an unsigned char (1-byte) value
       * @param command_info A JSON object that provides information about a command.
       * @param type_signifier An unsigned char to return as a default value to the function.
       * @return unsigned char type from the resulting operation or the default value
       */
      pin_size_t parseCommand(JsonObject command_info, pin_size_t type_signifier);
      /**
       * @brief Parse a command that should return a JsonArray value
       * @param command_info A JSON object that provides information about a command.
       * @param type_signifier An enum to specify the method overload. 
       * This is used to delay construction of a JsonArray object
       * @return JsonArray reference from the resulting operation or the default value
       */
      JsonArray parseCommand(JsonObject command_info, ArrayStatus type_signifier);

      /**
       * @note I am absolutely violating DRY principles because I need stack memory for recursion :/
       */


  };



};


#endif