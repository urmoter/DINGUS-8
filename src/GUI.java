import javafx.application.Application;
import javafx.application.Platform;
import javafx.geometry.Insets;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.ListView;
import javafx.scene.control.TextArea;
import javafx.scene.input.KeyCode;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;

public class GUI extends Application {

    private Cpu cpu;
    private TextArea terminal;
    private Label currentInstructionLabel;
    private final javafx.collections.ObservableList<String> stackViewItems =
            javafx.collections.FXCollections.observableArrayList();

    @Override
    public void init() {
        String binFile = getParameters().getRaw().get(0);
        cpu = new Cpu(binFile);
    }


    @Override
    public void start(Stage stage) {
        // Current instruction label
        currentInstructionLabel = new Label("Instruction: N/A");
        currentInstructionLabel.setStyle("-fx-font-family: monospace; -fx-font-size: 14px;");
        currentInstructionLabel.textProperty().bind(cpu.currentInstructionWithArgsProperty());

        // Register ListView
        ListView<String> registerView = new ListView<>();
        registerView.setItems(cpu.getRegisterStrings());
        registerView.setPrefHeight(180);

        // Stack ListView
        ListView<String> stackListView = new ListView<>(cpu.getStackViewItems());
        stackListView.setPrefHeight(150);

        // Terminal
        terminal = new TextArea();
        terminal.setEditable(false);
        terminal.setPrefHeight(200);

        cpu.setOutputConsumer(text ->
                Platform.runLater(() -> terminal.appendText(text)));

        // Flag table
        GridPane flagGrid = new GridPane();
        flagGrid.setHgap(10);
        flagGrid.setVgap(5);

        // Header row
        String[] flagNames = {"L", "Z", "N", "E", "C", "P", "G", "H"};

        for (int i = 0; i < flagNames.length; i++) {
            Label label = new Label(flagNames[i]);
            label.setStyle("-fx-font-family: monospace; -fx-font-weight: bold;");
            flagGrid.add(label, i, 0);
        }

        // Value row
        for (int i = 0; i < flagNames.length; i++) {
            Label value = new Label();
            value.setStyle("-fx-font-family: monospace;");
            value.textProperty().bind(cpu.getFlagProperty(i));
            flagGrid.add(value, i, 1);
        }

        // Buttons
        Button stepButton = new Button("Step");
        stepButton.setOnAction(e -> cpu.step());

        Button runButton = new Button("Run");
        runButton.setOnAction(e -> {
            cpu.setRunning(true);
            Thread t = new Thread(() -> {
                while (cpu.isRunning() && !cpu.isHalted()) {
                    cpu.step();
                    try { Thread.sleep(5); } catch (InterruptedException ignored) {}
                }
            });
            t.setDaemon(true);
            t.start();
        });

        Button stopButton = new Button("Stop");
        stopButton.setOnAction(e -> cpu.setRunning(false));

        HBox buttonRow = new HBox(10, stepButton, runButton, stopButton);

        // Layout: instruction, registers, stack, terminal, buttons
        VBox root = new VBox(
                10,
                currentInstructionLabel,
                registerView,
                flagGrid,
                stackListView,
                terminal,
                buttonRow
        );
        root.setPadding(new Insets(10));

        Scene scene = new Scene(root, 450, 650);
        stage.setScene(scene);
        stage.setTitle("CPU Debugger");
        stage.show();

        // Keyboard shortcut for stepping
        scene.setOnKeyPressed(event -> {
            if (event.getCode() == KeyCode.SPACE) cpu.step();
        });
    }


    public static void main(String[] args) {
        launch(args);
    }
}
