# Custom Shell Interpreter

This project is a custom Unix-like shell interpreter implemented in C++. It supports basic command execution, piping, and redirection, providing a simplified environment to execute shell commands.

## Features

- **Command Execution:** Execute standard Unix commands within the custom shell environment.
- **Piping:** Chain multiple commands using the pipe (`|`) operator.
- **Redirection:** Redirect input and output using `>` and `<` operators.

## Getting Started

### Prerequisites

- C++ compiler supporting C++11 or later.
- Unix-like operating system (Linux, macOS).

### Installation

1. **Clone the Repository:**

   ```bash
   git clone https://github.com/halelitzhaki/custom-shell-interpreter.git
   ```

2. **Navigate to the Project Directory:**

   ```bash
   cd custom-shell-interpreter
   ```

3. **Compile the Program:**

   ```bash
   make
   ```

   This will generate an executable named `custom_shell`.

### Running the Shell

Execute the compiled program:

```bash
./custom_shell
```

You will enter the custom shell environment, where you can input commands as you would in a standard Unix shell.

## Usage Examples

- **Simple Command:**

  ```bash
  ls -l
  ```

- **Piping:**

  ```bash
  ls -l | grep ".cpp"
  ```

- **Redirection:**

  ```bash
  echo "Hello, World!" > output.txt
  cat < output.txt
  ```

## Project Structure

- `src/`: Contains the source code files for the shell implementation.
- `include/`: Header files.
- `Makefile`: Facilitates the build process.
- `README.md`: Provides an overview and instructions for the project.

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Author

Hale Litzhaki

For any questions or suggestions, please feel free to contact me.
