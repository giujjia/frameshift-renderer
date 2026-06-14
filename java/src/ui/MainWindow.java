package ui;

import javax.swing.*;
import java.awt.*;
import java.awt.event.AdjustmentEvent;
import java.awt.event.AdjustmentListener;

public class MainWindow extends JFrame {
	private JTextField txtTranscriptId;
	private JTextField txtMutationHgvs;
	private JButton btnVisualizar;
	private JButton btnIrMutacao;
	private JScrollBar scrollBarCamera;
	private model.ProteinResult resultadoAtual;

	public MainWindow() {
		super("Painel de Controle - ShiftGL");
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setSize(500, 250);
		setLayout(new BorderLayout(10, 10));

		JPanel pnlInputs = new JPanel(new GridLayout(3, 2, 5, 5));
		pnlInputs.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));

		pnlInputs.add(new JLabel("ID Transcrito:"));
		txtTranscriptId = new JTextField("NM_001037732.2");
		pnlInputs.add(txtTranscriptId);

		pnlInputs.add(new JLabel("Mutacao HGVS:"));
		txtMutationHgvs = new JTextField("c.183_184insA");
		pnlInputs.add(txtMutationHgvs);

		btnVisualizar = new JButton("Visualizar");
		pnlInputs.add(btnVisualizar);

		btnIrMutacao = new JButton("Ir para Mutacao");
		pnlInputs.add(btnIrMutacao);

		add(pnlInputs, BorderLayout.CENTER);

		scrollBarCamera = new JScrollBar(JScrollBar.HORIZONTAL, 0, 100, 0, 3100);
		add(scrollBarCamera, BorderLayout.SOUTH);

		configurarListeners();
	}

	private void configurarListeners() {
		btnVisualizar.addActionListener(e -> {
			String nmId = txtTranscriptId.getText().trim();
			String hgvs = txtMutationHgvs.getText().trim();

			if (nmId.isEmpty() || hgvs.isEmpty()) {
				JOptionPane.showMessageDialog(this, "Preencha todos os campos", "Erro", JOptionPane.ERROR_MESSAGE);
				return;
			}

			// busque os metadados do transcrito no tsv do "nm_cds_positions.tsv", use o metodo findCdsMetadata da classe FileLoader passando o nmId
			
			// carregue o transcrito do arquivo fasta com o metodo findFastaSequence da classe FileLoader passando o nmId
			
			// use o metodo parse da classe HGVSParser passando o hgvs
			
			// crie o objeto do tipo Transcript passando o id o dna o id da proteina a posicao inicial e a posicao final do cds
			
			// instancie a classe Translator, chamando o metodo process passando o transcript e a mutation criados
			// salve o retorno na variavel resultadoAtual da classe
			
			// envie os dados processados para a ponte native do opengl
			// chame o metodo prepare da classe OpenGLBridge passando o resultadoAtual o nmId e o hgvs

			try {
				if (resultadoAtual != null) {
					// recalcula limite maximo do scrollbar
					int numAminoacidos = resultadoAtual.getProteinReference().length();
					int larguraTotal = numAminoacidos * 83;
					scrollBarCamera.setMaximum(larguraTotal);

					// reseta scroll da camera
					scrollBarCamera.setValue(0);
				}
			} catch (Exception ex) {
				JOptionPane.showMessageDialog(this, "Erro ao configurar limites: " + ex.getMessage(), "Erro", JOptionPane.ERROR_MESSAGE);
			}
		});

		btnIrMutacao.addActionListener(e -> {
			if (resultadoAtual == null) {
				JOptionPane.showMessageDialog(this, "Nenhum resultado processado ainda", "Aviso", JOptionPane.WARNING_MESSAGE);
				return;
			}
			int divIndex = resultadoAtual.getFirstDivergentPosition();
			if (divIndex >= 0) {
				// centraliza camera na mutacao se houver
				float cx = divIndex * 83.0f + 69.0f / 2.0f;
				float offset = Math.max(0.0f, cx - (1000.0f - 34.0f) / 2.0f);
				scrollBarCamera.setValue((int) offset);
				jni.OpenGLBridge.moveCamera(offset);
			} else {
				JOptionPane.showMessageDialog(this, "Nao ha posicao de mutacao divergente", "Informacao", JOptionPane.INFORMATION_MESSAGE);
			}
		});

		scrollBarCamera.addAdjustmentListener(new AdjustmentListener() {
			@Override
			public void adjustmentValueChanged(AdjustmentEvent e) {
				float offset = (float) e.getValue();
				// envia deslocamento horizontal da camera para motor
				jni.OpenGLBridge.moveCamera(offset);
			}
		});
	}
}
