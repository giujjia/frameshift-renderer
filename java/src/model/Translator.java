package model;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Map;

public class Translator {
	private final Map<String, String> TABLE_CODONS;

	public Translator() {
	    try {
	        this.TABLE_CODONS = FileLoader.loadCodonTable();
	    } catch (IOException e) {
	        throw new RuntimeException("Erro ao carregar tabela de códons: ", e);
	    }
	}
    
	//translates the sequence into a protein
    public String translate(String sequence) {
		StringBuilder protein = new StringBuilder();
		String codon; String aminoacid;
		
		for(int i = 0 ; i + 2 < sequence.length() ; i+=3) {
			//ACCAGCC
			codon = sequence.substring(i, i+3);
			aminoacid = TABLE_CODONS.get(codon);
			
			if(aminoacid.equals("STOP")) break; //if you encounter a stop codon, stop 
			
			protein.append(aminoacid);
		}
		return protein.toString();
	}
    
    // return the cdsMutation
    public String applyMutation(Mutation mutation, String cdsRef){
        int pos_mutation = mutation.getPosition() - 1;
        
        String before = cdsRef.substring(0, pos_mutation);
        String after = cdsRef.substring(pos_mutation + mutation.getDeleteLength());
    
        return before + mutation.getInsertSeq() + after;
    }
    
    
    
    public int[] divergenceTwoProtein(String protein_reference, String protein_mutation) {
    	char[] reference = protein_reference.toCharArray();
    	char[] mutation = protein_mutation.toCharArray();
    	
    	ArrayList<Integer> divergence = new ArrayList<>();
    	
    	int tamR = reference.length; int tamM = mutation.length;
    	
    	int indexR = 0,indexM = 0;
    	//se na mesma posicao houve altereçao de nucleotideos, a sequencia possui divergencia
    	while(indexR < tamR && indexM < tamM) {
    		if(reference[indexR] != mutation[indexM]) { 
    			divergence.add(indexR); 
    		}
    		indexR++; indexM++;
    	}
    	
    	while(indexR < tamR) {
    		divergence.add(indexR);
    		indexR++;
    	}
    	while(indexM < tamM) {
    		divergence.add(indexM);
    		indexM++;
    	}
    		
    	return divergence.stream().mapToInt(Integer::intValue).toArray();
    	//converte arrayList Inteiro para int
    }
    
    //armazena todos os processos e devolve um objeto com os dados
    public ProteinResult process(Transcript transcript, Mutation mutation) {
    	String cds_reference = transcript.getCdsRegion();
    	String cds_mutation = applyMutation(mutation,cds_reference);
    	
    	String protein_reference = translate(cds_reference);
    	String protein_mutation = translate(cds_mutation);
    	
    	boolean isFrameshift = mutation.isFrameshift();
    	
    	int[] divergentPositions = divergenceTwoProtein(protein_reference, protein_mutation);
    	int firstDivergentPosition = 
    			divergentPositions.length > 0 ? divergentPositions[0] : -1;
    	
    	int insertedBaseIndex = -1;
    	if (mutation.getDeleteLength() == 0 && !mutation.getInsertSeq().isEmpty()) {
    		insertedBaseIndex = mutation.getPosition() - 1;
    	}
    
    	return new ProteinResult(
    			cds_reference,
    			cds_mutation,
    			protein_reference,
    			protein_mutation,
    			isFrameshift,
    			firstDivergentPosition,
    			divergentPositions,
    			insertedBaseIndex
    	);
    }
}
