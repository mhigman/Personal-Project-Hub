#include "Parser.h"

namespace AppInventor
{
    // Intialize pins for the DCMotor wrapper class
    DCMotor::DCMotor(pin_size_t p1, pin_size_t p2) : pin1(p1), pin2(p2)
    {
    }

    // A default constructor
    DCMotor::DCMotor()
    {
        pin1 = 0;
        pin2 = 0;
    }

    void DCMotor::power(pin_size_t speed)
    {
        if (abs(speed) > 240)
        {
            speed = speed < 0 ? -240 : 240;
        }
        if (speed > 0)
        {
            analogWrite(pin1, speed);
            analogWrite(pin2, 0);
        }
        else
        {
            analogWrite(pin1, 0);
            analogWrite(pin2, speed);
        }
    }

    // Constructor
    Parser::Parser()
    {
    }

    void Parser::parseCommands(JsonObject comms, InIterator in_iterator)
    {
        for (JsonPair kv : comms)
        {
            const char *current_command = kv.key().c_str();

            if (comms[current_command].is<JsonObject>())
            {
                JsonObject command_info = comms[current_command];
                parseCommand(command_info);
            }
        }

        //     //Do stuff with commands here
    }

    void Parser::parseCommand(JsonObject command_info)
    {

        Subroutine command_code = command_info["code"];

        switch (command_code)
        {
        case Subroutine::FOR_RANGE:
        {

            JsonObject stepCommand = command_info["step"];
            JsonObject startCommand = command_info["start"];
            JsonObject stopCommand = command_info["stop"];

            double step = parseCommand(stepCommand, (double)0);
            double start = parseCommand(startCommand, (double)0);
            double stop = parseCommand(stopCommand, (double)0);

            range_iter_index++;
            for (double i = start; i < stop; i += step)
            {
                buffer_range_iter[range_iter_index] = i;
                parseCommands(command_info["action"]);
            }
            range_iter_index--;
        }
        break;
        case Subroutine::FOR_LIST:
        {
            const BasicDataType list_type = static_cast<BasicDataType>(command_info["list_type"]);

            switch (list_type)
            {

            case BasicDataType::INT:
            {
                JsonArray list = parseCommand(command_info["list"].as<JsonObject>(), ArrayStatus::ARRAY);

                element_int_index++;
                for (JsonVariant element : list)
                {
                    buffer_element_int_like[element_int_index] = element.as<int>();
                    parseCommands(command_info["action"]);
                }
                element_int_index--;
            }
            break;

            case BasicDataType::DOUBLE:
            {
                JsonArray list = parseCommand(command_info["list"].as<JsonObject>(), ArrayStatus::ARRAY);

                element_double_index++;
                for (JsonVariant element : list)
                {
                    buffer_element_int_like[element_double_index] = element.as<double>();
                    parseCommands(command_info["action"]);
                }
                element_double_index--;
            }
            break;

            case BasicDataType::BOOL:
            {
                JsonArray list = parseCommand(command_info["list"].as<JsonObject>(), ArrayStatus::ARRAY);

                element_bool_index++;
                for (JsonVariant element : list)
                {
                    buffer_element_bool[element_bool_index] = element.as<bool>();
                    parseCommands(command_info["action"]);
                }
                element_bool_index--;
            }
            break;

            case BasicDataType::CHAR:
            {
                JsonArray list = parseCommand(command_info["list"].as<JsonObject>(), ArrayStatus::ARRAY);

                element_char_index++;
                for (JsonVariant element : list)
                {
                    buffer_element_int_like[element_char_index] = element.as<pin_size_t>();
                    parseCommands(command_info["action"], InIterator::YES);
                }
                element_char_index--;
            }
            break;
            case BasicDataType::LIST:
            {      
                JsonArray list = parseCommand(command_info["list"].as<JsonObject>(), ArrayStatus::ARRAY);

                element_list_index++;
                for (JsonArray element : list)
                {
                    buffer_element_list[element_list_index] = element;
                    parseCommands(command_info["action"], InIterator::YES);
                }
                element_list_index--;
            }
            break;
            default:
                break;
            }
        }
        break;
        case Subroutine::WHILE:
        {
            // @todo optimize I guess. I bet we could get less function
            //      calls for the condition with some function refs.
            bool output = parseCommand(command_info["condition"], false);

            while (output)
            {
                parseCommands(command_info["action"], InIterator::NO);
                output = parseCommand(command_info["condition"], false);
            }
        }
        break;
        case Subroutine::IF:
        {
            bool output = parseCommand(command_info["condition"], false);

            if (output)
            {
                parseCommands(command_info["action"], InIterator::NO);
            }
        }
        break;
        case Subroutine::IF_ELSE:
        {
            bool output = parseCommand(command_info["condition"], false);

            if (output)
            {
                parseCommands(command_info["action1"], InIterator::NO);
            }
            else
            {
                parseCommands(command_info["action2"], InIterator::NO);
            }
        }
        break;
        case Subroutine::IF_ELIF_ELSE:
        {
            bool output1 = parseCommand(command_info["condition1"], false);
            bool output2 = parseCommand(command_info["condition2"], false);

            if (output1)
            {
                parseCommands(command_info["action1"], InIterator::NO);
            }
            else if (output2)
            {
                parseCommands(command_info["action2"], InIterator::NO);
            }
            else
            {
                parseCommands(command_info["action3"], InIterator::NO);
            }
        }
        break;
        /**
         * @note Servo and Stepper haven't been tested yet,
         *      and more functionality for these motors can (and should)
         *      be added.
         */
        case Subroutine::INIT_SERVO:
        {
            if (command_info.containsKey("name"))
            {
                unsigned char name = command_info["name"];
                buffer_servo[name].attach(
                    parseCommand(command_info["port"], (unsigned char)0));
            }
            else
            {
                buffer_servo[servo_index].attach(
                    parseCommand(command_info["port"], (unsigned char)0));
                servo_index++;
            }
        }
        break;
        case Subroutine::MOVE_SERVO_MOTOR:
        {
            unsigned char identifier = command_info["id"];
            int degree_value = parseCommand(command_info["degrees"], (int)0);
            buffer_servo[identifier].write(degree_value);
        }
        break;
        case Subroutine::INIT_STEPPER:
        {
            if (command_info.containsKey("name"))
            {
                unsigned char name = command_info["name"];
                buffer_stepper[name] = AccelStepper(
                    AccelStepper::DRIVER,
                    parseCommand(command_info["pin1"], (unsigned char)0),
                    parseCommand(command_info["pin2"], (unsigned char)0));
            }
            else
            {
                buffer_stepper[stepper_index] = AccelStepper(
                    AccelStepper::DRIVER,
                    parseCommand(command_info["pin1"], (unsigned char)0),
                    parseCommand(command_info["pin2"], (unsigned char)0));
                stepper_index++;
            }
        }
        break;
        case Subroutine::MOVE_STEPPER_MOTOR:
        {
            unsigned char port = command_info["id"];
            buffer_stepper[port].moveTo(
                parseCommand(command_info["position"], (int)0));
            buffer_stepper[port].runToPosition();
        }
        break;
        case Subroutine::INIT_MOTOR:
        {
            pin_size_t pin_1 = parseCommand(command_info["pin1"], (unsigned char)0);
            pin_size_t pin_2 = parseCommand(command_info["pin2"], (unsigned char)0);
            if (command_info.containsKey("name"))
            {
                unsigned char name = command_info["name"];
                buffer_motor[name] = DCMotor(pin_1, pin_2);
            }
            else
            {
                buffer_motor[motor_index] = DCMotor(pin_1, pin_2);
                motor_index++;
            }
        }
        break;
        case Subroutine::MOVE_DC_MOTOR:
        {
            unsigned char motorid = command_info["id"];
            buffer_motor[motorid].power(parseCommand(command_info["speed"], (int)0));
        }
        break;
        case Subroutine::DIGITAL_SWITCH_UP:
        {
            pin_size_t pin = parseCommand(command_info["number"], (pin_size_t)0);
            pinMode(pin, OUTPUT);
            digitalWrite(
                pin,
                HIGH);
        }
        break;
        case Subroutine::DIGITAL_SWITCH_DOWN:
        {
            pin_size_t pin = parseCommand(command_info["number"], (pin_size_t)0);
            pinMode(pin, OUTPUT);
            digitalWrite(
                pin,
                LOW);
        }
        break;
        case Subroutine::WRITE_TO_PORT_ANALOG:
        {
            analogWrite(
                parseCommand(command_info["number"], (pin_size_t)0),
                parseCommand(command_info["value"], (unsigned char)0));
        }
        break;
        case Subroutine::VARIABLE_SET:
        {

            /// @todo better error handling for running out of space.
            /// @todo DONT REPEAT MYSELF; However, consdiering typing,
            ///    I can at best do a template, which could be error prone
            /// @note I need space on call stack :sob: so lets keep this.
            const BasicDataType type = static_cast<BasicDataType>(command_info["type"]);
            switch (type)
            {
            case BasicDataType::INT:
                if (int_index >= 20)
                {
                    return;
                }
                else
                {
                    if (!command_info.containsKey("name"))
                    {
                        buffer_int_like[int_index] = parseCommand(command_info["value"], (int)0);
                        int_index++;
                    }
                    else
                    {
                        unsigned char name = command_info["name"];
                        buffer_int_like[name] = parseCommand(command_info["value"], (int)0);
                    }
                }
                break;
            case BasicDataType::BOOL:
                if (bool_index >= 40)
                {
                    return;
                }
                else
                {
                    if (!command_info.containsKey("name"))
                    {
                        buffer_bool[bool_index] = parseCommand(command_info["value"], (bool)0);
                        bool_index++;
                    }
                    else
                    {
                        unsigned char name = command_info["name"];
                        buffer_bool[name] = parseCommand(command_info["value"], (bool)0);
                    }
                }
                break;
            case BasicDataType::DOUBLE:
                if (double_index >= 60)
                {
                    return;
                }
                else
                {
                    if (!command_info.containsKey("name"))
                    {
                        buffer_int_like[double_index] = parseCommand(command_info["value"], (double)0);
                        double_index++;
                    }
                    else
                    {
                        unsigned char name = command_info["name"];
                        buffer_int_like[name] = parseCommand(command_info["value"], (double)0);
                    }
                }
                break;

            case BasicDataType::CHAR:
                if (char_index >= 80)
                {
                    return;
                }
                else
                {
                    if (!command_info.containsKey("name"))
                    {
                        buffer_int_like[char_index] = parseCommand(command_info["value"], (unsigned char)0);
                        char_index++;
                    }
                    else
                    {
                        unsigned char name = command_info["name"];
                        buffer_int_like[name] = parseCommand(command_info["value"], (unsigned char)0);
                    }
                }
                break;
            // @todo - list moment
            case BasicDataType::LIST:
                if (list_index >= 30)
                {
                    return;
                }
                else
                {
                    if (!command_info.containsKey("name"))
                    {
                        buffer_list[list_index] = parseCommand(command_info["value"], ArrayStatus::ARRAY);
                        int first_val = buffer_list[0][0];
                        list_index++;
                    }
                    else
                    {
                        unsigned char name = command_info["name"];
                        buffer_list[name] = parseCommand(command_info["value"], ArrayStatus::ARRAY);
                    }
                }
                break;
            default:
                break;
            }
        }
        break;
        /**
         * @todo Make many communication methods. For now, I have deliverables to make :skull:
         *          (May not get to this before paper stuff. I think its more important to document)
         */
        case Subroutine::SEND:
        {
            CommunicationMethod method = static_cast<CommunicationMethod>(command_info["method"]);
            switch (method)
            {
            case CommunicationMethod::COMM_SERIAL:
            {
                /**
                 * @todo Add support for arrays for data sending to optimize command space
                 */
                for (JsonPair kv : command_info)
                {
                    const char *name = kv.key().c_str();
                    if (strcmp(name, "code") && strcmp(name, "method"))
                    {

                        Serial.print(parseCommand(kv.value().as<JsonObject>(), (double)0));
                        Serial.print(',');
                    }
                }
                Serial.println();
            }
            break;
            case CommunicationMethod::COMM_BLUETOOTH:
            {
                /**
                 * @todo Add support for arrays for data sending to optimize command space
                 */
                if (writePacket != nullptr)
                {
                    String final_string = "";
                    for (JsonPair kv : command_info)
                    {
                        const char *name = kv.key().c_str();
                        if (strcmp(name, "code") && strcmp(name, "method"))
                        {
                            final_string += parseCommand(kv.value().as<JsonObject>(), (double)0);
                            final_string += ',';
                        }
                    }
                    writePacket->setValue(final_string);
                }
            }
            break;
            case CommunicationMethod::COMM_WIFI:
            {
                /**
                 * @todo implement later.
                 */
            }
            break;
            default:
                break;
            }
        }
        break;
        case Subroutine::WAIT:
        {
            int time = abs(parseCommand(command_info["time"], (int)0));
            delay(time);
        }
        break;
        default:
        {
        }
        break;
        }
    }

    bool Parser::parseCommand(JsonObject command_info, bool type_signifier)
    {

        Subroutine command_code = command_info["code"];

        switch (command_code)
        {
        case Subroutine::AND:
        {
            return parseCommand(command_info["arg1"], false) && parseCommand(command_info["arg2"], false);
        }
        break;
        case Subroutine::OR:
        {
            return parseCommand(command_info["arg1"], false) || parseCommand(command_info["arg2"], false);
        }
        break;
        case Subroutine::NOT:
        {
            return !(parseCommand(command_info["arg"], false));
        }
        break;
        case Subroutine::EQUAL_TO:
        {
            return parseCommand(command_info["arg1"], (double)0) == parseCommand(command_info["arg2"], (double)0);
        }
        break;
        case Subroutine::NOT_EQUAL_TO:
        {
            return parseCommand(command_info["arg1"], (double)0) != parseCommand(command_info["arg2"], (double)0);
        }
        break;
        case Subroutine::GREATER_THAN:
        {
            return parseCommand(command_info["arg1"], (double)0) > parseCommand(command_info["arg2"], (double)0);
        }
        break;
        case Subroutine::LESS_THAN:
        {
            double arg1 = parseCommand(command_info["arg1"], (double)0);
            double arg2 = parseCommand(command_info["arg2"], (double)0);
            return arg1 < arg2;
        }
        break;
        case Subroutine::GREATER_THAN_EQUAL:
        {
            return parseCommand(command_info["arg1"], (double)0) >= parseCommand(command_info["arg2"], (double)0);
        }
        break;
        case Subroutine::LESS_THAN_EQUAL:
        {
            return parseCommand(command_info["arg1"], (double)0) <= parseCommand(command_info["arg2"], (double)0);
        }
        break;
        case Subroutine::VARIABLE_CALL:
        {
            if (command_info.containsKey("iter"))
            {
                String iterator_variable = String(command_info["iter"]);
                char depth = iterator_variable.charAt(iterator_variable.length() - 1);
                return buffer_element_bool[depth - 65];
            }
            unsigned char name = command_info["name"];
            return buffer_bool[name];
        }
        break;
        case Subroutine::LITERAL:
        {
            return command_info["value"].as<bool>();
        }
        break;
        default:

            break;
        }
        return type_signifier;
    }

    int Parser::parseCommand(JsonObject command_info, int type_signifier)
    {

        Subroutine command_code = command_info["code"];

        switch (command_code)
        {
        case Subroutine::VARIABLE_CALL:
        {
            if (command_info.containsKey("iter"))
            {
                String iterator_variable = String(command_info["iter"]);
                char depth = iterator_variable.charAt(iterator_variable.length() - 1);
                return static_cast<int>(buffer_element_int_like[depth - 65]);
            }
            else if (command_info.containsKey("iter_range"))
            {
                String iterator_variable = String(command_info["iter_range"]);
                char depth = iterator_variable.charAt(iterator_variable.length() - 1);
                return static_cast<int>(buffer_range_iter[depth - 65]);
            }
            unsigned char name = command_info["name"];
            return static_cast<int>(buffer_int_like[name]);
        }
        break;
        case Subroutine::LITERAL:
        {
            return command_info["value"].as<int>();
        }
        break;
        default:
            return static_cast<int>(parseCommand(command_info, (double)0));
            break;
        }
        return type_signifier;
    }

    double Parser::parseCommand(JsonObject command_info, double type_signifier)
    {
        Subroutine command_code = command_info["code"];

        switch (command_code)
        {
        case Subroutine::VARIABLE_CALL:
        {
            if (command_info.containsKey("iter"))
            {
                String iterator_variable = String(command_info["iter"]);
                char depth = iterator_variable.charAt(iterator_variable.length() - 1);
                return buffer_element_int_like[depth - 65];
            }
            else if (command_info.containsKey("iter_range"))
            {
                String iterator_variable = String(command_info["iter_range"]);
                char depth = iterator_variable.charAt(iterator_variable.length() - 1);
                return static_cast<double>(buffer_range_iter[depth - 65]);
            }
            unsigned char name = command_info["name"];
            return buffer_int_like[name];
        }
        break;
        case Subroutine::LITERAL:
        {
            return command_info["value"].as<double>();
        }
        break;
        case Subroutine::ADD:
        {
            return parseCommand(command_info["arg1"], (double)0) + parseCommand(command_info["arg2"], (double)0);
        }
        break;
        case Subroutine::SUBTRACT:
        {
            return parseCommand(command_info["arg1"], (double)0) - parseCommand(command_info["arg2"], (double)0);
        }
        break;
        case Subroutine::MULTIPLY:
        {
            return parseCommand(command_info["arg1"], (double)0) * parseCommand(command_info["arg2"], (double)0);
        }
        break;
        case Subroutine::DIVIDE:
        {
            return parseCommand(command_info["arg1"], (double)0) / parseCommand(command_info["arg2"], (double)0);
        }
        break;
        default:

            break;
        }
        return type_signifier;
    }

    pin_size_t Parser::parseCommand(JsonObject command_info, pin_size_t type_signifier)
    {
        Subroutine command_code = command_info["code"];

        switch (command_code)
        {
        case Subroutine::VARIABLE_CALL:
        {
            if (command_info.containsKey("iter"))
            {
                String iterator_variable = String(command_info["iter"]);
                char depth = iterator_variable.charAt(iterator_variable.length() - 1);
                return static_cast<pin_size_t>(buffer_element_int_like[depth - 65]);
            }
            unsigned char name = command_info["name"];
            return static_cast<unsigned char>(buffer_int_like[name]);
        }
        break;
        case Subroutine::READ_FROM_PORT_ANALOG:
        {
            return analogRead(
                parseCommand(command_info["number"], (unsigned char)0));
        }
        break;
        case Subroutine::READ_FROM_PORT_DIGITAL:
        {
            unsigned char pin = parseCommand(command_info["number"], (unsigned char)0);
            pinMode(pin, INPUT);
            return static_cast<int>(digitalRead(pin));
        }
        break;
        case Subroutine::LITERAL:
            return command_info["value"].as<pin_size_t>();
            break;
        default:
            return static_cast<unsigned char>(parseCommand(command_info, (double)0));
            break;
        }
        return type_signifier;
    }

    JsonArray Parser::parseCommand(JsonObject command_info, ArrayStatus type_signifier)
    {
        Subroutine command_code = command_info["code"];

        switch (command_code)
        {
        case Subroutine::VARIABLE_CALL:
        {
            if (command_info.containsKey("iter"))
            {
                String iterator_variable = String(command_info["iter"]);
                char depth = iterator_variable.charAt(iterator_variable.length() - 1);
                return buffer_element_list[depth - 65];
            }

            unsigned char name = command_info["name"];
            return buffer_list[name];
        }
        break;
        case Subroutine::LITERAL:
            return command_info["value"].as<JsonArray>();
            break;
        default:

            break;
        }
        return JsonArray();
    }

    void Parser::prepareCommands(char (&command_feed)[3000], BLEStringCharacteristic *packet)
    {

        JsonDocument jsonDump;

        DeserializationError error = deserializeJson(jsonDump, command_feed);

        if (error)
        {
            Serial.println(error.c_str());
            return;
        }
        // Serial.println("Given JSON:");
        // serializeJson(jsonDump, Serial);
        // Serial.println();
        writePacket = packet;

        jsonDump.shrinkToFit();

        JsonObject objectDump = jsonDump.as<JsonObject>();

        parseCommands(objectDump, InIterator::NO);
    }

}