package model;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.HashMap;
import java.util.Map;

public class FileLoader {
   private static final String PASTA_DADOS = "data";
   
    static boolean isEqualsIdLine(String id, String line) {
		///ex:>NR_107042.1 Homo sapiens microRNA 8075 (MIR8075), microRNA
		String idProcurado =  line.substring(1, line.indexOf(" ")); //extract in range > and last id
		return id.equals(idProcurado);
	}
    //this method finds the sequence of codons based on the ID being searched for
    static String findFastaSequence(String id) {
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
