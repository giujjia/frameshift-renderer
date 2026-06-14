package main;

import model.*;
import jni.OpenGLBridge;

public class Main {

	public static void main(String[] args) {
		String nmId = "";
		String npId = "";
		int cdsStart = 0;
		int cdsEnd = 0;
		String hgvs = "";

		if (args.length == 1) {
			String opcao = args[0];
			if (opcao.equals("1")) {
				nmId = "NM_001037732.2";
				hgvs = "c.183_184insA";
			} else if (opcao.equals("2")) {
				nmId = "NM_004737.5";
				hgvs = "c.165delG";
			} else if (opcao.equals("3")) {
				nmId = "NM_001130106.1";
				hgvs = "c.75_76insCGGC";
			} else if (opcao.equals("4")) {
				nmId = "NM_033089.6";
				hgvs = "c.461_462delCGinsGCG";
			} else {
				imprimirInstrucoes();
				return;
			}
		} else if (args.length == 2) {
			nmId = args[0];
			hgvs = args[1];
		} else if (args.length >= 5) {
			nmId = args[0];
			npId = args[1];
			try {
				cdsStart = Integer.parseInt(args[2]);
				cdsEnd = Integer.parseInt(args[3]);
			} catch (NumberFormatException e) {
				System.err.println("[JAVA] Erro: CDS_START e CDS_END devem ser inteiros");
				return;
			}
			hgvs = args[4];
		} else {
			imprimirInstrucoes();
			return;
		}

		if (args.length == 1 || args.length == 2) {
			String[] metadata = FileLoader.findCdsMetadata(nmId);
			if (metadata == null) {
				System.err.println("[JAVA] Erro: Metadados nao encontrados para " + nmId);
				return;
			}
			npId = metadata[1];
			try {
				cdsStart = Integer.parseInt(metadata[2]);
				cdsEnd = Integer.parseInt(metadata[3]);
			} catch (NumberFormatException e) {
				System.err.println("[JAVA] Erro ao converter posicoes CDS do arquivo TSV");
				return;
			}
		}

		System.out.println("[JAVA] Iniciando o Frameshift Renderer...");
		System.out.println("[JAVA] Buscando sequencia de nucleotideos no arquivo FASTA...");
		String sequence = FileLoader.findFastaSequence(nmId);
		if (sequence == null) {
			System.err.println("[JAVA] Erro: Transcrito " + nmId + " nao encontrado no FASTA");
			return;
		}
		System.out.println("[JAVA] Sequencia carregada. Tamanho: " + sequence.length() + " nucleotideos");

		Transcript transcript = new Transcript(nmId, sequence, npId, cdsStart, cdsEnd);

		System.out.println("[JAVA] Interpretando mutacao: " + hgvs);
		Mutation mutation = HGVSParser.parse(hgvs);
		if (mutation == null) {
			System.err.println("[JAVA] Erro: Notacao HGVS invalida ou nao suportada");
			return;
		}

		System.out.println("[JAVA] Processando mutacao e traduzindo proteinas...");
		Translator translator = new Translator();
		ProteinResult result = translator.process(transcript, mutation);

		int lenRef = result.getProteinReference().length();
		int lenMut = result.getProteinMutation().length();
		String printRef = lenRef > 15 ? result.getProteinReference().substring(0, 15) + "..." : result.getProteinReference();
		String printMut = lenMut > 15 ? result.getProteinMutation().substring(0, 15) + "..." : result.getProteinMutation();

		System.out.println("[JAVA] Resultado do Processamento:");
		System.out.println("  - Proteina Ref: " + printRef + " (" + lenRef + " aa)");
		System.out.println("  - Proteina Mut: " + printMut + " (" + lenMut + " aa)");
		System.out.println("  - Frameshift:   " + (result.isFrameshift() ? "Sim" : "Nao"));
		System.out.println("  - Primeiro AA divergente: " + (result.getFirstDivergentPosition() + 1));

		System.out.println("[JAVA] Enviando dados para a ponte JNI C++...");
		OpenGLBridge.prepare(result, nmId, hgvs);

		System.out.println("[JAVA] Abrindo a janela grafica do OpenGL na thread principal...");
		try {
			OpenGLBridge.openWindow();
		} catch (UnsatisfiedLinkError e) {
			System.err.println("[JAVA] Erro JNI: Nao foi possivel carregar a biblioteca nativa");
			e.printStackTrace();
		} catch (Exception e) {
			System.err.println("[JAVA] Erro ao abrir a janela OpenGL: " + e.getMessage());
			e.printStackTrace();
		}
	}

	private static void imprimirInstrucoes() {
		System.out.println("ShiftGL - Visualizador de Mutacoes e Frameshifts");
		System.out.println("\nUso 1 (Caso de Teste rapido):");
		System.out.println("  java -cp build -Djava.library.path=../native/build main.Main <1|2|3|4>");
		System.out.println("\nUso 2 (Busca automatica de CDS no TSV):");
		System.out.println("  java -cp build -Djava.library.path=../native/build main.Main <NM_ID> <MUTACAO_HGVS>");
		System.out.println("\nUso 3 (Parametros manuais completos):");
		System.out.println("  java -cp build -Djava.library.path=../native/build main.Main <NM_ID> <NP_ID> <CDS_START> <CDS_END> <MUTACAO_HGVS>");
		System.out.println("\nExemplos:");
		System.out.println("  java -cp build -Djava.library.path=../native/build main.Main 1");
		System.out.println("  java -cp build -Djava.library.path=../native/build main.Main NM_001037732.2 c.183_184insA");
	}
}
