import base64
import requests

# CONFIGURAÇÕES
# Token da conta SECUNDÁRIA (com permissão de repo)
TOKEN = "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"
OWNER = "brunnojob"  # O teu username principal (dono do repo)
REPO = "pid-pressure-control"  # Nome do repositório público
SECONDARY_USERNAME = "ineedfoundmyway"
SECONDARY_ID = "329826984"

HEADERS = {
    "Authorization": f"Bearer {TOKEN}",
    "Accept": "application/vnd.github+json",
    "X-GitHub-Api-Version": "2022-11-28",
}

API_URL = f"https://api.github.com/repos/{OWNER}/{REPO}"


def get_main_sha():
  res = requests.get(f"{API_URL}/git/ref/heads/main", headers=HEADERS)
  return res.json()["object"]["sha"]


def create_branch(branch_name, sha):
  data = {"ref": f"refs/heads/{branch_name}", "sha": sha}
  res = requests.post(f"{API_URL}/git/refs", json=data, headers=HEADERS)
  return res.status_code == 201


def get_file_content():
  res = requests.get(f"{API_URL}/contents/src/controller.cpp", headers=HEADERS)
  return res.json()["sha"], res.json()["content"]


def update_file_and_create_pr(i, base_sha, file_sha, original_content):
  branch_name = f"pair-update-{i}"

  # Criar a branch
  if not create_branch(branch_name, base_sha):
    print(f"Erro ao criar branch {branch_name}")
    return

  # Modificar ligeiramente o conteúdo (adicionar um comentário numerado útil)
  decoded_bytes = base64.b64decode(original_content)
  decoded_text = decoded_bytes.decode("utf-8")

  # Inserir um comentário único no final do ficheiro para diferenciar o commit
  new_content_text = (
      f"{decoded_text}\n// Iteration check optimization tag #{i}\n"
  )
  encoded_content = base64.b64encode(new_content_text.encode("utf-8")).decode(
      "utf-8"
  )

  # Mensagem de commit com a co-autoria correta da conta principal
  commit_msg = (
      f"refactor: optimize controller loop iteration {i}\n\nCo-authored-by:"
      f" {OWNER} <{SECONDARY_ID}+{OWNER}@users.noreply.github.com>"
  )

  # Atualizar o ficheiro na nova branch
  update_data = {
      "message": commit_msg,
      "content": encoded_content,
      "sha": file_sha,
      "branch": branch_name,
  }
  put_res = requests.put(
      f"{API_URL}/contents/src/controller.cpp",
      json=update_data,
      headers=HEADERS,
  )

  if put_res.status_code not in [200, 201]:
    print(f"Erro ao atualizar ficheiro na branch {branch_name}")
    return

  # Abrir o Pull Request
  pr_data = {
      "title": f"Pair programming optimization #{i}",
      "head": branch_name,
      "base": "main",
      "body": (
          "Automated pair programming PR for achievement progression."
          f" Iteration {i}."
      ),
  }
  pr_res = requests.post(f"{API_URL}/pulls", json=pr_data, headers=HEADERS)

  if pr_res.status_code == 201:
    print(f"[SUCESSO] PR #{i} aberto com sucesso!")
  else:
    print(f"[ERRO] Falha ao abrir PR #{i}: {pr_res.text}")


def main():
  print("A obter dados do repositório...")
  main_sha = get_main_sha()
  file_sha, file_content = get_file_content()

  print("A iniciar a criação dos 10 PRs com co-autoria...")
  for i in range(1, 11):
    update_file_and_create_pr(i, main_sha, file_sha, file_content)


if __name__ == "__main__":
  main()