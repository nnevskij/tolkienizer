class Tolkienizer < Formula
  desc "Transmute ordinary words into the mythic prose of J.R.R. Tolkien using AI"
  homepage "https://github.com/nnevskij/tolkienizer"
  url "https://github.com/nnevskij/tolkienizer/archive/refs/tags/v1.1.0.tar.gz"
  sha256 "ce45fc82b0a18df99c600aaff9eba45bcb64000332ac08a5b4ee5ec7aba2d38d"
  license "MIT"
  head "https://github.com/nnevskij/tolkienizer.git", branch: "main"

  depends_on "pkg-config" => :build
  depends_on "cjson"
  depends_on "curl"

  def install
    system "make"
    bin.install "tolkienizer"
  end

  test do
    assert_match "tolkienizer version", shell_output("#{bin}/tolkienizer --version")
    assert_match "epic", shell_output("#{bin}/tolkienizer --list-styles")
    assert_match "English", shell_output("#{bin}/tolkienizer --list-langs")
  end
end
