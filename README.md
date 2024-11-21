# Custom Shell Interpreter

This project is a custom Unix-like shell interpreter implemented in C. It supports basic command execution, piping, and redirection, providing a simplified environment to execute shell commands.

## Features

- **Command Execution:** Execute standard Unix commands within the custom shell environment.
- **Piping:** Chain multiple commands using the pipe (`|`) operator.
- **Redirection:** Redirect input and output using `>` and `<` operators.

## Getting Started

### Prerequisites

- A Linux environment (Ubuntu or other distributions).
- GCC compiler.

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

   This will generate an executable named `myshell`.

### Running the Shell

Execute the compiled program:

```bash
./myshell
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

- `myshell.c`: Source code for the shell implementation.
- `source.h`: The source code header file.
- `Makefile`: Facilitates the build process.
- `README.md`: Provides an overview and instructions for the project.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Acknowledgments

This project was developed as part of an academic assignment to practice advanced C and OS programming concepts.

## Author

**Halel Itzhaki**

For any questions or suggestions, please feel free to contact me.

