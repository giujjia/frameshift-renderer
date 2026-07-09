package model;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class FileLoader {
	private static final String PASTA_DADOS = "data";
	private static final String HISTORICO = "Historico.txt";
	static boolean isEqualsIdLine(String id, String line){
		String idLinha = line.substring(1, line.indexOf(" "));
		String baseProcurado = id.contains(".") ? id.substring(0, id.indexOf(".")) : id;
		String baseLinha = idLinha.contains(".") ? idLinha.substring(0, idLinha.indexOf(".")) : idLinha;
		return baseProcurado.equalsIgnoreCase(baseLinha);
	}

	public static String findFastaSequence(String id){
		//refseqHumanFullNM.fasta
		Path caminhoArquivo = Path.of(PASTA_DADOS,"refseqHumanFullNM.fasta");
		StringBuilder sequence = new StringBuilder();
		String line;

		try(BufferedReader read = Files.newBufferedReader(caminhoArquivo)){
			while((line = read.readLine()) != null) { // repeat util the end of file
				if(line.startsWith(">")) {
					if(isEqualsIdLine(id,line)) {
						while((line = read.readLine()) != null && !line.startsWith(">")) {
							sequence.append(line); // link the sequence of nucleotide
						}
						return sequence.toString();
					}
				}
			}
		}catch(Exception erro) {
			System.out.println(erro.getMessage());
		}
		return null;
	}

	public static String[] findCdsMetadata(String id) {
		String baseId = id.contains(".") ? id.substring(0, id.indexOf(".")) : id;
		Path caminhoArquivo = Path.of(PASTA_DADOS, "nm_cds_positions.tsv");
		String line;

		try (BufferedReader read = Files.newBufferedReader(caminhoArquivo)) {
			read.readLine();
			while ((line = read.readLine()) != null) {
				String[] parts = line.split("\t");
				if (parts.length >= 4) {
					String currentNm = parts[0];
					String currentNmBase = currentNm.contains(".") ? currentNm.substring(0, currentNm.indexOf(".")) : currentNm;
					if (baseId.equalsIgnoreCase(currentNmBase)) {
						return parts;
					}
				}
			}
		} catch (Exception erro) {
			System.err.println(erro.getMessage());
		}
		return null;
	}

	
	public static ArrayList<String> abrir_historico() {
	    Path caminhoArquivo = Path.of(PASTA_DADOS, HISTORICO);
	    ArrayList<String> lista = new ArrayList<>();

	    try {
	        Files.createDirectories(caminhoArquivo.getParent());

	        if (!Files.exists(caminhoArquivo)) {
	            Files.createFile(caminhoArquivo);
	            return lista;
	        }

	        try (BufferedReader read = Files.newBufferedReader(caminhoArquivo)) {
	            String line;
	            while ((line = read.readLine()) != null) {
	                lista.add(line);
	            }
	        }

	    } catch (Exception erro) {
	        erro.printStackTrace();
	    }

	    return lista;
	}
	
	
	public static void salvar_historico(ArrayList<String> lista) {
	    if (lista == null) return;

	    Path caminhoArquivo = Path.of(PASTA_DADOS, HISTORICO);

	    try {
	        Files.createDirectories(caminhoArquivo.getParent());

	        try (BufferedWriter writer = Files.newBufferedWriter(caminhoArquivo)) {
	            for (String l : lista) {
	                writer.append(l);
	                writer.newLine();
	            }
	        }

	    } catch (Exception erro) {
	        erro.printStackTrace();
	    }
	}
	

    
    /* nao sei se vai ser usado
    static boolean isEqualsTsv(String id, String line) {
		//NM_001368254.1	NP_001355183.1	47	1195	human.1.rna.gbff.gz
		String idProcurado = line.substring(0, line.indexOf("\t"));
		return id.equals(idProcurado);
	}
    static String[] matchIndex(String id) {
		String caminhoArquivo = "C:\\Users\\Usuário\\Documents\\GitHub\\frameshift-demo#\\data\\nm_cds_positions.tsv";
		String line;
		
		try(BufferedReader read = new BufferedReader(new FileReader(caminhoArquivo))){
			while((line = read.readLine()) != null) {
				if(isEqualsTsv(id,line)) {
					String[] block = line.split("\t");
					return block;
				}
			}
			
			
		}catch(Exception erro) {
			System.out.println(erro.getMessage());
		}
		return null; 
	}
	*/
	
    
	//this method instantiates TABLE_CODONS for the Translator class
	static Map<String,String> loadCodonTable() throws IOException{
		Path caminhoArquivo = Path.of(PASTA_DADOS,"codons.tsv");

		String line; String codon; String aminoacid;
		Map<String,String> TABLE_CODONS = new HashMap<>();

		try(BufferedReader read = Files.newBufferedReader(caminhoArquivo)){
			while((line = read.readLine())!= null) {
				//table:TTT	F
				String[] parts = line.split("\t");
				if(parts.length != 2) {
					throw new IOException("Linha invalida a tabela de codons"+line);
				}
				codon 		= parts[0]; 
				aminoacid 	= parts[1];
				TABLE_CODONS.put(codon, aminoacid);
			}
		}
		return TABLE_CODONS;

	}

}
