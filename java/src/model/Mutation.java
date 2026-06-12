package model;


//essa classe equivale à tupla do código em python. Armazena informaçõe sobre a operação
//inicio, tamanho da deleção e a sequência que foi inserida.
public class Mutation {
	private final int position;
	private final int deleteLength;
	private final String insertSeq;
	private final MutationType type;

	public Mutation(int position, int deleteLength, String insertSeq,
		MutationType type){
			this.position		= position;
			this.deleteLength	= deleteLength;
			this.insertSeq		= insertSeq;
			this.type			= type;

		}

	//frameshift ocorre quando a mudança é multipla de 3
	public boolean isFrameshift() {
		int change = insertSeq.length() - deleteLength;
		if(change % 3 != 0){
			return true;
		}else{
			return false;
		}
	}
	
	
	public int getPosition() {
		return this.position;
	}

	public int getDeleteLength() {
		return deleteLength;
	}

	public String getInsertSeq() {
		return insertSeq;
	}
}
