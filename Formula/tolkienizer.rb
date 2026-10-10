class Tolkienizer < Formula
  desc "Transmute ordinary words into the mythic prose of J.R.R. Tolkien using AI"
  homepage "https://github.com/nnevskij/tolkienizer"
  url "https://github.com/nnevskij/tolkienizer/archive/refs/tags/v1.0.0.tar.gz"
  sha256 "1a3ea7e3b378619d4830d9639353e93629193e274cfed0876054382ebbcaacfb"
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
