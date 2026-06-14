package main;

import javax.swing.SwingUtilities;
import ui.MainWindow;
import jni.OpenGLBridge;

public class Main {

	public static void main(String[] args) {
		// inicializa a interface Swing na thread EDT
		SwingUtilities.invokeLater(() -> {
			MainWindow frame = new MainWindow();
			frame.setLocationRelativeTo(null);
			frame.setVisible(true);
		});

		// executa o loop nativo GLFW
		try {
			OpenGLBridge.openWindow();
		} catch (UnsatisfiedLinkError e) {
			e.printStackTrace();
		}
	}
}