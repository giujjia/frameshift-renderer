package main;

import model.FileLoader;
import model.HGVSParser;
import model.Transcript;
import model.Mutation;
import model.Translator;
import model.ProteinResult;
import jni.OpenGLBridge;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import java.awt.EventQueue;

import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.border.EmptyBorder;
import javax.swing.JLabel;
import javax.swing.JOptionPane;

import java.awt.Font;
import javax.swing.JTextField;
import java.awt.Color;
import javax.swing.JSeparator;
import javax.swing.SwingConstants;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JButton;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import javax.swing.JScrollBar;
import javax.swing.JTextPane;
import javax.swing.border.LineBorder;
import java.awt.event.AdjustmentListener;
import java.awt.event.AdjustmentEvent;

public class MainWindow extends JFrame {

	private static final long serialVersionUID = 1L;
	private JPanel contentPane;
	private JTextField entradaID;
	private JTextField entradaHGVS;
	private ProteinResult resultadoAtual;
	private JTextPane visREsultado;
	private JScrollBar scrollBar;
	private JButton btnIrMutacao;
	private JButton btnVisualizar;
	private JScrollPane Resultado;
	private boolean janelaAberta = false;




 
	//main - inicia aplicação
	public static void main(String[] args) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					MainWindow frame = new MainWindow();
					frame.setVisible(true);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}

	//definição de componentes e ação da tela
	public MainWindow() {
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setBounds(100, 100, 1002, 399);
		contentPane = new JPanel();
		contentPane.setBackground(new Color(255, 255, 255));
		contentPane.setBorder(new EmptyBorder(5, 5, 5, 5));
		setContentPane(contentPane);
		contentPane.setLayout(null);
		
		//Implementação dos componentes básicos
		JPanel panel = new JPanel();
		panel.setBackground(new Color(233, 241, 247));
		panel.setBounds(12, 12, 978, 74);
		contentPane.add(panel);
		panel.setLayout(null);
		
		JLabel tPag = new JLabel("ShiftGL - Frameshift Mutation Visualizer");
		tPag.setFont(new Font("FiraCode Nerd Font Med", Font.BOLD, 18));
		tPag.setBounds(224, 24, 462, 22);
		panel.add(tPag);
		
		JPanel panel_1 = new JPanel();
		panel_1.setBorder(new LineBorder(new Color(0, 0, 0)));
		panel_1.setBackground(new Color(255, 255, 255));
		panel_1.setBounds(12, 98, 978, 204);
		contentPane.add(panel_1);
		panel_1.setLayout(null);
		
		JLabel txtTranscriptId = new JLabel("ID do transcrito");
		txtTranscriptId.setFont(new Font("Dialog", Font.BOLD, 15));
		txtTranscriptId.setBounds(12, 12, 246, 17);
		panel_1.add(txtTranscriptId);
		
		entradaID = new JTextField();
		entradaID.setFont(new Font("Dialog", Font.PLAIN, 14));
		entradaID.setBounds(12, 41, 273, 27);
		panel_1.add(entradaID);
		entradaID.setColumns(10);
		
		JLabel txtMutationHgvs = new JLabel("Mutação HGVS:  ");
		txtMutationHgvs.setFont(new Font("Dialog", Font.BOLD, 15));
		txtMutationHgvs.setBounds(12, 74, 246, 17);
		panel_1.add(txtMutationHgvs);
		
		JLabel lblNewLabel_2 = new JLabel("Resultado");
		lblNewLabel_2.setFont(new Font("Dialog", Font.BOLD, 15));
		lblNewLabel_2.setBounds(604, 12, 80, 17);
		panel_1.add(lblNewLabel_2);
		
		entradaHGVS = new JTextField();
		entradaHGVS.setFont(new Font("Dialog", Font.PLAIN, 14));
		entradaHGVS.setBounds(12, 101, 273, 27);
		panel_1.add(entradaHGVS);
		entradaHGVS.setColumns(10);
		
		JLabel lblNewLabel = new JLabel("Utilize para mover a câmera");
		lblNewLabel.setFont(new Font("FiraCode Nerd Font Light", Font.BOLD, 14));
		lblNewLabel.setBounds(360, 314, 263, 17);
		contentPane.add(lblNewLabel);
		
		
		//RESULTADO -------------------------------------------------------------------------------------------------------
		Resultado = new JScrollPane();
		Resultado.setBounds(309, 41, 645, 132);		
		panel_1.add(Resultado);		
		visREsultado = new JTextPane();
		visREsultado.setFont(new Font("FiraCode Nerd Font Light", Font.PLAIN, 12));
		visREsultado.setEditable(false);
		Resultado.setViewportView(visREsultado);
		
		
		
		//SCROLLBAR --------------------------------------------------------------------------------------------------------
		scrollBar = new JScrollBar(JScrollBar.HORIZONTAL,0,100,0,100);		
		scrollBar.setToolTipText("Visualize uma mutação para habilitar este controle");
		scrollBar.setOrientation(JScrollBar.HORIZONTAL);
		scrollBar.setBounds(12, 342, 978, 17);
		
		contentPane.add(scrollBar);
		//ação do botão - move a camera
		scrollBar.addAdjustmentListener(new AdjustmentListener() {
			@Override	
			public void adjustmentValueChanged(AdjustmentEvent e) {
				float offset = (float) e.getValue();
				// envia deslocamento horizontal da camera para motor
				OpenGLBridge.moveCamera(offset);	
			}
		});

		
		//MUTAÇÃO --------------------------------------------------------------------------------------------------------
		btnIrMutacao = new JButton("Ver Mutação");
		btnIrMutacao.setToolTipText("Visualize uma mutação para habilitar este botão");
		btnIrMutacao.setEnabled(false);
		btnIrMutacao.setBackground(new Color(233, 241, 247));
		btnIrMutacao.addActionListener(new ActionListener() {
			//ação do botão 
			public void actionPerformed(ActionEvent arg0) {
				//tratamento de erro resultado null
				if(resultadoAtual == null) {
					mostrarAviso("Nenhuma mutação processada ainda");
					return;
				}else {
					btnIrMutacao.setToolTipText("Abrir Frameshit Mutation Visualizer");

				}

					int divIndex = resultadoAtual.getFirstDivergentPosition();
					if (divIndex >= 0) {
						float cx = divIndex * 83.0f + 69.0f / 2.0f;
						float offset = Math.max(0.0f, cx - (1000.0f - 34.0f) / 2.0f);		
						abrirVisualizacao(offset);						
					} else {
						mostrarAviso("Não há posição de mutação divergente");
					}
				}
		});
		btnIrMutacao.setBounds(168, 146, 117, 27);
		panel_1.add(btnIrMutacao);
		
		//VISUALIZAR ------------------------------------------------------------------------------------------------
		btnVisualizar = new JButton("Visualizar");
		btnVisualizar.setBackground(new Color(233, 241, 247));
		btnVisualizar.setBounds(12, 146, 117, 27);
		panel_1.add(btnVisualizar);
		
		btnVisualizar.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				String id = entradaID.getText().trim();
				String hgvs = entradaHGVS.getText().trim();
				
				if(id.isBlank() || hgvs.isBlank()) {
					mostrarAviso("Preencha todos os campos");
					return;
				}
				
				
				//processa mutação
				processarMutacao(id,hgvs);
							
				//reconfigura ScrollBar
				configurarScrollBar(resultadoAtual);
				
				// mostra resultado na tela
				exibirResultado(resultadoAtual);
				
				//ponte
				OpenGLBridge.prepare(resultadoAtual, id, hgvs);
				
				//se ponte feita com sucesso habilita botão
				btnIrMutacao.setEnabled(true);

			}
			
		});
		
	}
	//MÉTODOS -------------------------------------------------------------------------------------------------
	private void abrirVisualizacao(float offset) {

		    if (janelaAberta)
		        return;

		    janelaAberta = true;

		    Thread openglThread = new Thread(() -> {
		        try {
		            OpenGLBridge.openWindow();
		        } finally {
		            janelaAberta = false;
		        }
		    });

		    openglThread.start();

		    scrollBar.setValue((int) offset);
	}
	
	private void processarMutacao(String id, String hgvs) {
		//pega dados do tvs
		String[] metadado = FileLoader.findCdsMetadata(id);
		if(metadado == null) {
			mostrarErro("Transcrito não encontrado");
			return;
		}
		
		//pega sequencia
		String dna = FileLoader.findFastaSequence(id);
		if(dna == null) {
			mostrarErro("Sequência não encontrada");
			return;
		}
		
		//criação do transcript
		Transcript transcript = new Transcript(
				metadado[0],                   // nmId
			    dna,                           // sequência
			    metadado[1],                   // npId
			    Integer.parseInt(metadado[2]), // cds_start
			    Integer.parseInt(metadado[3])  // cds_end
			    );  
		//criação da mutação
		Mutation mutation = HGVSParser.parse(hgvs);
		if(mutation == null) {
			JOptionPane.showMessageDialog(
				MainWindow.this,
				"Mutação HGVS inválida",
				"Erro",
				JOptionPane.ERROR_MESSAGE
			);
			return;
		}
		//Translator
		Translator translator = new Translator();
		resultadoAtual = translator.process(transcript, mutation);
		if(resultadoAtual == null) {
			JOptionPane.showMessageDialog(
				MainWindow.this,
				"Não foi possível processar a mutação",
				"Erro",
				JOptionPane.ERROR_MESSAGE
			);
			return;
		}
	}
	
	private void configurarScrollBar(ProteinResult resultadoAtual) {
		try {
			if (resultadoAtual != null) {
				// recalcula limite maximo do scrollbar
				if(resultadoAtual != null && resultadoAtual.getProteinReference() != null) {
					int numAminoacidos = resultadoAtual.getProteinReference().length();
					int larguraTotal = numAminoacidos * 83;
					scrollBar.setValues(
						    0,
						    824,
						    0,
						    larguraTotal
						);		
				}
				// reseta scroll da camera
				scrollBar.setValue(0);
			}
		} catch (Exception ex) {
			mostrarErro("Erro ao configurar limites");
		}
		
	}
	
	private void exibirResultado(ProteinResult resultado) {
		String resultadoTexto =
				  "<html><body style='font-family:\"FiraCode Nerd Font\"; font-size:10px;'>" +

				    "<b>Proteína de referência:</b><br>" +
				    resultadoAtual.getProteinReference() +
				    "<br><br><b>Proteína mutante:</b><br>" +
				    resultadoAtual.getProteinMutation() +

				    "<br><br><b>Posição de divergência:</b> " +
				    resultadoAtual.getFirstDivergentPosition() +

				    "<br><br><b>Frameshift:</b> " +
				    resultadoAtual.isFrameshift() +
				    "</html>";
			
			visREsultado.setText(resultadoTexto);
			visREsultado.setContentType("text/html");
			visREsultado.setText(resultadoTexto);
	}
	 
	private void mostrarErro(String mensagem) {
		JOptionPane.showMessageDialog(MainWindow.this, 
				mensagem, "Erro", 
				JOptionPane.ERROR_MESSAGE);
	}
	
	private void mostrarAviso(String mensagem) {
		JOptionPane.showMessageDialog(MainWindow.this,
				mensagem, "Alerta",
				JOptionPane.WARNING_MESSAGE);	
	}

	

}

	


