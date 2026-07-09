package main;


import java.util.ArrayList;
import model.FileLoader;

public class HistoricoWindow {
	ArrayList<String> historico;
	public HistoricoWindow() {
		this.historico = FileLoader.abrir_historico(); 
	}
	public void adicionar_historico(String IdTranscrito, String Mutacao) {
		String elemento = IdTranscrito + "|" + Mutacao;
		historico.add(elemento);
	}
	public String buscar_historico(int idex) {
		return historico.get(idex);
	}
	public void limpar_historico() {
		historico.clear();
	}
	public void salvar() {
		FileLoader.salvar_historico(historico);
	}
	public ArrayList<String> getHistorico() {
	    return historico;
	}
}
