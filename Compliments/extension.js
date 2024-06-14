// The module 'vscode' contains the VS Code extensibility API

// Import the module and reference it with the alias vscode in your code below
const vscode = require('vscode');

// This method is called when your extension is activated
// Your extension is activated the very first time the command is executed

/*async function get_things_from_document(){
	return vscode.commands.executeCommand(
				'vscode.executeDocumentSymbolProvider',
				vscode.window.activeTextEditor.document.uri
			);
}*/

console.log(vscode.workspace.getConfiguration("compliments").listofcompliments);

/**
 * @param {vscode.ExtensionContext} context
 */
async function activate(context) {
	
	// Use the console to output diagnostic information (console.log) and errors (console.error)
	// This line of code will only be executed once when your extension is activated
	console.log('Congratulations, your extension "palm-extension" is now active!');
	/*
	const symbolsToFind = [
		vscode.SymbolKind.Function, 
		vscode.SymbolKind.Method,
		vscode.SymbolKind.Constructor,
	];
	const functionDecorationType = vscode.window.createTextEditorDecorationType({
		border: 'none',
		before:{
			contentText: "(: This is a function, method, or constructor.\r\nW",
			height: "50px",
		}
	});
	*/

	const listOfMotivationalQuotes = vscode.workspace.getConfiguration("compliments").listofcompliments;
	let firstDecorationType = vscode.window.createTextEditorDecorationType({
		//borderWidth: '1px',
		//borderStyle: 'solid',
		//borderSpacing: '10px',
		isWholeLine: true,
		
		light: {
			//borderColor: 'darkblue'
			backgroundColor: '#9cfff566',
			after: {
				contentText: listOfMotivationalQuotes[
					Math.floor(listOfMotivationalQuotes.length * Math.random())
				],
				margin: "10px",
				color: '#0f0f0f'
			},
		},
		dark: {
			//borderColor: 'lightblue'
			backgroundColor: '#002d3066',
			after: {
				contentText: listOfMotivationalQuotes[
					Math.floor(listOfMotivationalQuotes.length * Math.random())
				],
				margin: "10px",
				color: "#f0f0f0"
			},
		},
		//textDecoration: ';padding:0.05rem',	
	});

	let prevLine = vscode.window.activeTextEditor.selection.active.line;

	let motivationalQuotes = vscode.window.onDidChangeTextEditorSelection(() => {
		const listOfMotivationalQuotes = vscode.workspace.getConfiguration("compliments").listofcompliments;
		const activeEditor = vscode.window.activeTextEditor;
		//console.log(firstDecorationType.key);
		if (activeEditor) {
			if(activeEditor.selection.active.line !== prevLine) {

				firstDecorationType.dispose()
				firstDecorationType = vscode.window.createTextEditorDecorationType({
					//borderWidth: '0.5px',
					//borderStyle: 'dashed',
					//borderSpacing: '10px',
					isWholeLine: true,
					light: {
						//borderColor: 'darkblue'
						backgroundColor: '#9cfff66',
						after: {
							contentText: listOfMotivationalQuotes[
								Math.floor(listOfMotivationalQuotes.length * Math.random())
							],
							margin: "10px",
							color: '#0f0f0f0f'
						},
					},
					dark: {
						//borderColor: 'lightblue'
						backgroundColor: '#002d3066',
						after: {
							contentText: listOfMotivationalQuotes[
								Math.floor(listOfMotivationalQuotes.length * Math.random())
							],
							margin: "10px",
							color: "#f0f0f0"
						},
					},
				});


				prevLine = activeEditor.selection.active.line;
				activeEditor.setDecorations(firstDecorationType, []);
				const currentLineRange = [
					activeEditor.document.lineAt(activeEditor.selection.active.line).range
				];
				activeEditor.setDecorations(firstDecorationType, currentLineRange);
				
			}
		}
	});
	
	// The command has been defined in the package.json file
	// Now provide the implementation of the command with  registerCommand
	// The commandId parameter must match the command field in package.json
	let command = vscode.commands.registerCommand('compliments.helloWorld', async function () {
		// The code you place here will be executed every time your command is executed
		vscode.window.showInformationMessage('Hello World from compliments!');
			
	    /*let ranges = [];
			for(let i = 0; i < document.lineCount; i++){
					const line = document.lineAt(i);
					const range = new vscode.Range(line.range.start, line.range.end);

					if(i % 3 === 0){
						ranges.push(range);
					}
			}
			
			const symbols = await get_things_from_document();

			const functions = symbols ? 
				// @ts-ignore
				symbols.filter(symbol => symbolsToFind.includes(symbol.kind))
				: undefined;
			
			activeEditor.setDecorations(functionDecorationType, []);
			activeEditor.setDecorations(firstDecorationType, []);

			activeEditor.setDecorations(functionDecorationType, functions)
			activeEditor.setDecorations(firstDecorationType, ranges);*/
	});
	context.subscriptions.push(motivationalQuotes);
	context.subscriptions.push(command);
}

// This method is called when your extension is deactivated
function deactivate() {}

module.exports = {
	activate,
	deactivate
}
