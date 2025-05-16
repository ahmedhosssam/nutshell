package main

import (
	"fmt"
	"bufio"
	"os"
	"io"
	"strings"
	"os/exec"
)

func runCmd(line string) {
	if line == "\n" {
		return
	}

	tokens := strings.Fields(line)
	program := tokens[0]

	paths := strings.Split(os.Getenv("PATH"), ":")

	var execPath string = ""

	if tokens[0] == "cd" {
		err := os.Chdir(tokens[1])
		if err != nil {
			fmt.Println(err)
		}
		return
	}


	OuterLoop:
	for _, path := range paths {
		entries, _ := os.ReadDir(path)
		for _, entry := range entries {
			if entry.Name() == program {
				execPath += path
				execPath += "/"
				execPath += entry.Name()
				break OuterLoop
			}
		}
	}

	if execPath == "" {
		fmt.Println("Error: Command not found.")
		return
	}

	cmd := exec.Command(execPath, tokens[1:]...)
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr

	err := cmd.Run()

	if err != nil {
		fmt.Println(err)
		return
	}
}

func main() {

	reader := bufio.NewReader(os.Stdin)

	for {
		fmt.Print(">> ")

		input, err := reader.ReadString('\n')

		if err == io.EOF {
			fmt.Println("\nBye.")
			break;
		}

		runCmd(input)
	}
}
